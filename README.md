# c_mlogparser

```bash
~]$ ls
mongod.log #10GB file
~]$ head -n 1 mongod.lg
2016-10-06T06:32:13.457-0700 W NETWORK  [ReplicaSetMonitorWatcher] Failed to connect to 10.20.33.98:27017, reason: errno:111 Connection refuse
~]$ tail -n 1 mongod.lg
2016-12-07T06:39:46.826-0700 I NETWORK  [conn4361] end connection 10.60.2.211:44490 (441 connections now open)
~]$ time wc -l mongod.log #time to read from SSD on my current laptop
39839739 mongod.log

real	0m33.411s
user	0m2.717s
sys	0m7.023s
~]$ #wc command above would be ~3 secs if already fully paged into RAM

~]$ mlogfilter -m 2015-11-01 -n 2015-11-30 -l IWE -c REPL mongod.log
2016-11-01T06:32:13.457-0700 W NETWORK  [ReplicaSetMonitorWatcher] Failed to connect to 10.60.99.99:27017, reason: errno:111 Connection refused
2016-11-01T06:32:14.609-0700 W SHARDING [replSetDistLockPinger] pinging failed for distributed lock pinger :: caused by :: ReplicaSetNotFound: None of the hosts for replica set baReplset could be contacted.
...
2016-11-01T21:23:47.251-0700 I REPL     [ReplicationExecutor] syncing from primary: mFakeShard01.aw.corp:27017
2016-11-01T21:23:47.256-0700 I REPL     [SyncSourceFeedback] setting syncSourceFeedback to mFakeShard01.aw.corp:27017
2016-11-01T21:25:12.810-0700 I REPL     [ReplicationExecutor] Error in heartbeat request to mFakeShard01.aw.corp:27017; HostUnreachable: End of file
...
~]$ ls -A
mongod.log #nothing new yet
~]$ mlogfilter -s E mongod.log #A filter without time bounds will mean a cache can be made
2016-11-06T23:20:17.722-0700 E REPL     [rsBackgroundSync] network error while attempting to run command 'isMaster' on host 'mFakeShard04.aw.corp:27017' 
2016-11-06T23:24:13.759-0700 E REPL     [rsBackgroundSync] network error while attempting to run command 'isMaster' on host 'mFakeShard04.aw.corp:27017' 
2016-11-06T23:58:02.002-0700 E REPL     [rsBackgroundSync] network error while attempting to run command 'isMaster' on host 'mFakeShard04.aw.corp:27017' 
...
~]$ ls -A
mongod.log #10GB
.mlfc.mongod.log #compact cache of file
~]$ mlogfilter --help
The fast log filter for mongod and mongos log files. Restricted to simple
  string comparison logic. If there is --ts-start argument a bifurcated search
  will seek the beginning, i.e. avoid scanning the whole file. Without a
  --ts-start argument mlogfilter will create a cache file to make repeated
  searches faster.
Usage:
  mlogfilter [options] <mongod or mongos log file>
Options:
  --help This message
  -m, --ts-start
    Filter out log lines before this time. YYYY-MM-DDTHH:MM:SS format, 
    timezone of log file assumed.
  -n, --ts-end
    Filter out log lines after this time. YYYY-MM-DDTHH:MM:SS format, 
    timezone of log file assumed. Is inclusive, e.g '2016-09-14' will really
    mean < 2016-09-15T00:00:00
  -s, --severity
    One or more of I, W, E, D. Join without spaces if multiple wanted. (Usually
    not useful whilst doing broad search for events).
  -c, --component 
    Log component: (e.g. COMMAND, REPL, SHARDING, NETWORK). Comma-delimit to
    include multiple.
  -p, --thread-name
    Starting string for '[threadName]' filtering. Comma-delimit to include
    multiple. Case-sensitive. E.g. "conn,repl,Repl,rs" for both connection and
    replication threads.
```
