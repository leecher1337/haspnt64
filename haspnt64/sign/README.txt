Workaround for this dreaded driver-signing rip-off...

Prereq:
-------
 * OPENSSL 1.1 binaries in openssl\ subdir
 * PHP 7 or higher installed in c:\php
 * A driver signing certificate, can be expired.

Signing:
--------

1) Change system date back to whatever year used driver signing cert is valid.
2) startsig.cmd
3) Compile and sign driver

Create new CA:
--------------
1) Change system date back to beginning of validity of driver signing 
   certificate (at least some date before expiration of cert and BEFORE 
    the date you are using in step "Signing")
2) Run create.cmd in ts
   Be sure to enter different common names for CA and cert!
   When asked for password, enter 12345
   When asked for export password in last step, leave blank

Testing timstamp server:
-----------------------
test.cmd can test timestamp server
