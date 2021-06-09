

from os import system
import random, string
from datetime import datetime
from typing import OrderedDict
from copy import deepcopy
def randomword(length):
   letters = string.ascii_lowercase
   return ''.join(random.choice(letters) for i in range(length))

# import parse
import collections
def random_test( threads, _input ):
    words =  " ".join([randomword(100) for _ in range(_input)] )
    _test_input = "{0} {1} {2}".format( _input, threads, words)
    # system( "sh compile.sh")
    
    _file_name = "test{0}".format( datetime.now() )
    _file_name = str(_file_name).replace( " ", "-") 
    system( "echo 1 {0} | ./ptest >> {1}".format(_test_input, _file_name ))
    
    print("input :")
    print(deepcopy(words).replace(" ", "\n"))

    temp = ''.join(words).replace(' ', '')
    res = { } 
    for char in string.ascii_lowercase:
        if temp.count(char) > 0 :
            res[char] = temp.count(char)

    output = { } 
    for line in open( _file_name, 'r').readlines():
        if "The character" in line :
            cindex = len("The character ")
            iindex = len("The character s appeared ")
            iendindex = line[iindex:].find(" ")
            output[ line[cindex] ] = int(line[iindex: iindex + iendindex ]) 
    
    final =  [ OrderedDict(sorted(_obj.items())) for _obj in [ output, res ]]
    print(final)
    print(final[0] == final[1])
    return final[0] == final[1] , final


if __name__ == "__main__":
    for threds in range(20, 40):
        sucss, out = random_test( threds , 20)
        
