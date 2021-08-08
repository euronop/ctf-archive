import sys

##fileto_patch = r"c:\ctf\2011\bins\cleaner"
##patched_file = r"c:\ctf\2011\bins\cleaner.ipv4"
##patches = [ (0x11f6, 0x1c, 0x02),
##            (0x122B, 0x1c, 0x02),
##            (0x1268, 0x1c, 0x10) ]

fileto_patch = r"c:\ctf\2011\bins\tomato"
##patched_file = r"c:\ctf\2011\bins\tomato.ipv4"
##patches = [ (0x1336, 0x1c, 0x02),
##            (0x136b, 0x1c, 0x02),
##            (0x13a8, 0x1c, 0x10) ]

def readfile(fn):
    fh = open(fn, 'rb')
    data = fh.read()
    fh.close()
    return data

def find_patch_pos(fileto_patch):
    """Find positions to pach from IPV6 to IPV4
    """
    data = readfile(fileto_patch)
    tmp = []
    p = data.find('\x1C')
    while p != -1:
        if (data[p+53] == '\x1C') and (data[p+114] == '\x1C'):
            tmp.append(p)
        p = data.find('\x1C', p+1)
    print "Found", len(tmp), "positions"
    print "Patch position 1: 0x%08x" % tmp[0]
    if len(tmp) != 1:
        return -1
    return tmp[0]
    
def patch(fileto_patch): #, patched_file, patches):
    patched_file = fileto_patch + '.ipv4'
    patchpos = find_patch_pos(fileto_patch)
    if patchpos == -1:
        return
    patches = [ (patchpos, 0x1c, 0x02),
                (patchpos+53, 0x1c, 0x02),
                (patchpos+114, 0x1c, 0x10) ]
    data = readfile(fileto_patch)

    for patch in patches:
        offset, oldbyte, newbyte = patch
        co = chr(oldbyte)
        cn = chr(newbyte)
        if data[offset] != co:
            print "Byte to patch does not match", ord(data[offset])
            sys.exit(0)
        data = data[:offset] + cn + data[offset+1:]

    fh = open(patched_file, 'wb')
    fh.write(data)
    fh.close()
    print "File successfully patched"

patch(fileto_patch) #, patched_file, patches)
