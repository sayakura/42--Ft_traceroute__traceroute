# 42--Ft_traceroute__traceroute
Recode linux command traceroute.

# Usage

```sh
sudo ./ft_tracerotue hostname

sudo ./ft_tracerotue hostname -w 1 -q 4 -m 10 -S google.com
```

### Flags
| flags | Description |
| ------ | ------ |
| -m | [ change max ttl value ] |
| -q | [ set how many probes per hop, default 3 ] |
| -f | [ started from this hop, default 1 ] |
| -S | [ print out a message indicating how many packets were lost ] |
| -w | [ w set the wait time between probes, default 3 seconds ] |

### Bonus Feature
- DNS resolution for the ips and all the 5 flags.
