/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * src/tools/ifaddrs/test_ifaddrs.c
 *
 *
 *	test_ifaddrs.c
 *		test pg_foreach_ifaddr()
 */

#include "postgres.h"

#include <netinet/in.h>
#include <arpa/inet.h>

#include "libpq/ifaddr.h"


static void
print_addr(struct sockaddr *addr)
{
	char		buffer[256];
	int			ret,
				len;

	switch (addr->sa_family)
	{
		case AF_INET:
			len = sizeof(struct sockaddr_in);
			break;
#ifdef HAVE_IPV6
		case AF_INET6:
			len = sizeof(struct sockaddr_in6);
			break;
#endif
		default:
			len = sizeof(struct sockaddr_storage);
			break;
	}

	ret = getnameinfo(addr, len, buffer, sizeof(buffer), NULL, 0,
					  NI_NUMERICHOST);
	if (ret != 0)
		printf("[unknown: family %d]", addr->sa_family);
	else
		printf("%s", buffer);
}

static void
callback(struct sockaddr *addr, struct sockaddr *mask, void *unused)
{
	printf("addr: ");
	print_addr(addr);
	printf("  mask: ");
	print_addr(mask);
	printf("\n");
}

int
main(int argc, char *argv[])
{
#ifdef WIN32
	WSADATA		wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		fprintf(stderr, "WSAStartup failed\n");
		return 1;
	}
#endif

	if (pg_foreach_ifaddr(callback, NULL) < 0)
		fprintf(stderr, "pg_foreach_ifaddr failed: %s\n", strerror(errno));
	return 0;
}
