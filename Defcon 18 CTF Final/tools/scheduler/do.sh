#!/bin/sh
openssl s_client -connect 10.31.100.100:2525 -cert ./cert/team_2_key.cert -key ./cert/team_2_key -CAfile ./cert/server.cert
