import urllib3
import sys

TARGET = 'http://crypto-class.appspot.com/po?er='
# TARGET = 'http://www.baidu.com'
# --------------------------------------------------------------
# padding oracle
# --------------------------------------------------------------

http = urllib3.PoolManager()


class PaddingOracle(object):
    def query(self, q):
        target = TARGET + q                       # Create query URL
        req = http.request('GET', target)         # Send HTTP request to server
        print(req.status)                         # print status code


if __name__ == "__main__":
    po = PaddingOracle()

    # Issue HTTP query with the given argument
    po.query("f20bdba6ff29eed7b046d1df9fb7000058b1ffb4210a580f748b4ac714c001bd4a61044426fb515dad3f21f18aa577c0bdf302936266926ff37dbf7035d5eeb4")