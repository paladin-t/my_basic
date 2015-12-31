' This script is an example of MY-BASIC
' Copyright (c) 2011 - 2016 Wang Renxin. All rights reserved.
' For more information, see https://github.com/paladin-t/my_basic/

class animal
    def speak(a)
        print "default" + a;
    enddef
endclass

class cat(animal)
    def speak(a)
        print "meow" + a;
    enddef
endclass

class dog(animal)
    def speak(a)
        print "bark" + a;
    enddef
endclass

c = new(cat)
d = new(dog)

c.speak("!")
d.speak("!")