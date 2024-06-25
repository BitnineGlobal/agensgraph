# AgensGraph: Powerful Multi-Model Graph Database

![Build Status](https://github.com/bitnine-oss/agensgraph/actions/workflows/regression.yml/badge.svg)

<img src="img/logo.png" alt="AgensGraph Logo" width="200" />

AgensGraph is a cutting-edge multi-model graph database designed for modern complex data environments. By supporting both relational and graph data models simultaneously, AgensGraph allows developers to seamlessly integrate legacy relational data with the flexible graph data model within a single database. AgensGraph is built on the robust PostgreSQL RDBMS, providing a highly reliable, fully-featured platform ready for enterprise use.



## Key Features

- **Multi-Model Support**: Combines relational and graph data models.
- **Query Languages**: Supports ANSI-SQL and openCypher ([openCypher](http://www.opencypher.org)).
- **Seamless Integration**: Integrate SQL and Cypher queries within a single query.
- **Enterprise-Ready**: ACID transactions, multi-version concurrency control, stored procedures, triggers, constraints, sophisticated monitoring, and flexible data models (JSON).
- **Extensible**: Leverages PostgreSQL's rich ecosystem, including modules like PostGIS.



## Building from Source

Follow these steps to build AgensGraph from the source code:

1. **Clone the Repository**
    ```sh
    $ git clone https://github.com/bitnine-oss/agensgraph.git
    ```

2. **Install Dependencies**
    - **CentOS**:
        ```sh
        $ yum install gcc glibc glib-common readline readline-devel zlib zlib-devel
        ```
    - **Fedora**:
        ```sh
        $ dnf install gcc glibc bison flex readline readline-devel zlib zlib-devel
        ```
    - **RHEL**:
        ```sh
        $ yum install gcc glibc glib-common readline readline-devel zlib zlib-devel flex bison
        ```
    - **Ubuntu**:
        ```sh
        $ sudo apt-get install build-essential libreadline-dev zlib1g-dev flex bison
        ```
    - **macOS** (install Xcode):
        ```bash
        $ xcode-select --install
        ```

3. **Configure the Source Tree**
    ```sh
    $ ./configure --prefix=$(pwd)
    ```
   > By default, `make install` installs files in `/usr/local/pgsql/bin`, `/usr/local/pgsql/lib`, etc. Specify an installation prefix to the current directory. If `configure` encounters missing headers, use `--with-includes=/path/to/headers`.

4. **Build and Install AgensGraph**
    ```sh
    $ make install
    ```

5. **Set Up Environment Variables**
    - Add the install path to the `PATH` environment variable:
        ```sh
        $ . ag-env.sh
        ```
      OR edit your `/.bashrc` file (`/.bash_profile` on macOS):
        ```sh
        $ echo "export PATH=/path/to/agensgraph/bin:\$PATH" >> ~/.bashrc
        $ echo "export LD_LIBRARY_PATH=/path/to/agensgraph/lib:\$LD_LIBRARY_PATH" >> ~/.bashrc
        ```

6. **Optional: Build and Install with Additional Modules**
    ```sh
    $ make install-world
    ```

7. **Optional: Set `AGDATA` Environment Variable**
    ```sh
    $ echo "export AGDATA=/path/to/agensgraph/data" >> ~/.bashrc
    ```

## Documentation

- [Quick Guide](http://bitnine.net/documentations/quick-guide-1-3.html)

## License

- [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0)
