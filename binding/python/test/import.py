#!/usr/bin/env python

r'''
>>> import owlcpp.rdf
>>> ts = owlcpp.rdf.Triple_store()
'''

def run(args = None):
    if args is not None:
        import sys
        sys.argv = args
    import doctest
    return doctest.testmod(verbose=True)

if __name__ == '__main__':
    import sys
    sys.exit(run()[0])