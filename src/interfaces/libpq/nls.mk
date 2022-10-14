# src/interfaces/libpq/nls.mk
CATALOG_NAME     = libpq
AVAIL_LANGUAGES  = cs de el es fr ja ko ru sv uk zh_CN
GETTEXT_FILES    = fe-auth.c fe-auth-scram.c fe-connect.c fe-exec.c fe-gssapi-common.c fe-lobj.c fe-misc.c fe-protocol3.c fe-secure.c fe-secure-common.c fe-secure-gssapi.c fe-secure-openssl.c win32.c
GETTEXT_TRIGGERS = libpq_gettext pqInternalNotice:2
GETTEXT_FLAGS    = libpq_gettext:1:pass-c-format pqInternalNotice:2:c-format
