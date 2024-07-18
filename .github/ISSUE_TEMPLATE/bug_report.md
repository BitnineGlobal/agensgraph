---
name: Bug report
about: Create a report to help us resolve bug
title: ''
labels: bug
assignees: ''

---

**Describe the bug**
A clear and concise description of what the bug is.

**How are you accessing Agensgraph (Command line, driver, etc.)?**
- [e.g. JDBC]

**What data setup do we need to do?**
```pgsql
  CREATE (a:Part {part_num: '123'}), 
         (b:Part {part_num: '345'}), 
         (c:Part {part_num: '456'}), 
         (d:Part {part_num: '789'})
```

**What is the necessary configuration info needed?**
- [e.g. Installed PostGIS]

**What is the command that caused the error?**
```pgsql
MATCH (a:Part {part_num: '123'}), (b:Part {part_num: '345'})
CREATE (a)-[u:used_by { quantity: 1 }]->(b)
```
```
ERROR:  something failed to execute
```

**Expected behavior**
A clear and concise description of what you expected to happen.

**Environment (please complete the following information):**
- Version: [e.g. 2.13.2]

**Additional context**
Add any other context about the problem here.