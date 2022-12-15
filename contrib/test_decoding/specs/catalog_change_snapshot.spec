# Test decoding only the commit record of the transaction that have
# modified catalogs.
setup
{
    DROP TABLE IF EXISTS tbl1;
    CREATE TABLE tbl1 (val1 integer, val2 integer);
    CREATE TABLE user_cat (val1 integer) WITH (user_catalog_table = true);
}

teardown
{
    DROP TABLE tbl1;
    DROP TABLE user_cat;
    SELECT 'stop' FROM pg_drop_replication_slot('isolation_slot');
}

session "s0"
setup { SET synchronous_commit=on; }
step "s0_init" { SELECT 'init' FROM pg_create_logical_replication_slot('isolation_slot', 'test_decoding'); }
step "s0_begin" { BEGIN; }
step "s0_savepoint" { SAVEPOINT sp1; }
step "s0_truncate" { TRUNCATE tbl1; }
step "s0_insert" { INSERT INTO tbl1 VALUES (1); }
step "s0_insert2" { INSERT INTO user_cat VALUES (1); }
step "s0_commit" { COMMIT; }

session "s1"
setup { SET synchronous_commit=on; }
step "s1_checkpoint" { CHECKPOINT; }
step "s1_get_changes" { SELECT data FROM pg_logical_slot_get_changes('isolation_slot', NULL, NULL, 'skip-empty-xacts', '1', 'include-xids', '0'); }

# For the transaction that TRUNCATEd the table tbl1, the last decoding decodes
# only its COMMIT record, because it starts from the RUNNING_XACTS record emitted
# during the first checkpoint execution.  This transaction must be marked as
# containing catalog changes while decoding the COMMIT record and the decoding
# of the INSERT record must read the pg_class with the correct historic snapshot.
#
# Note that in a case where bgwriter wrote the RUNNING_XACTS record between "s0_commit"
# and "s0_begin", this doesn't happen as the decoding starts from the RUNNING_XACTS
# record written by bgwriter.  One might think we can either stop the bgwriter or
# increase LOG_SNAPSHOT_INTERVAL_MS but it's not practical via tests.
permutation "s0_init" "s0_begin" "s0_savepoint" "s0_truncate" "s1_checkpoint" "s1_get_changes" "s0_commit" "s0_begin" "s0_insert" "s1_checkpoint" "s1_get_changes" "s0_commit" "s1_get_changes"

# Test that we can handle the case where there is no association between top-level
# transaction and its subtransactions. The last decoding restarts from the first
# checkpoint, decodes NEW_CID generated by "s0_insert2", and marks the subtransaction
# as containing catalog changes while adding tuple cids to its top-level transaction.
# During that, both transaction entries are created in ReorderBuffer as top-level
# transactions and have the same LSN. We check if the assertion check for the order
# of transaction LSNs in AssertTXNLsnOrder() is skipped since we are still before the
# LSN at which we start replaying the contents of transactions. Besides, when decoding
# the commit record of the top-level transaction, we must force the top-level
# transaction to do timetravel since one of its subtransactions has been marked as
# containing catalog changes.
permutation "s0_init" "s0_begin" "s0_savepoint" "s0_insert" "s1_checkpoint" "s1_get_changes" "s0_insert2" "s0_commit" "s0_begin" "s0_insert" "s1_checkpoint" "s1_get_changes" "s0_commit" "s1_get_changes"

# The last decoding restarts from the first checkpoint and adds invalidation
# messages generated by "s0_truncate" to the subtransaction. While
# processing the commit record for the top-level transaction, we decide
# to skip this xact but ensure that corresponding invalidation messages
# get processed.
permutation "s0_init" "s0_begin" "s0_savepoint" "s0_insert" "s1_checkpoint" "s1_get_changes" "s0_truncate" "s0_commit" "s0_begin" "s0_insert" "s1_checkpoint" "s1_get_changes" "s0_commit" "s1_get_changes"
