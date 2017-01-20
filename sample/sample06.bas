' This script is an example of MY-BASIC
' Copyright (c) 2011 - 2017 Wang Renxin. All rights reserved.
' For more information, see https://github.com/paladin-t/my_basic/

class animal
	def speak(a)
		print "Default" + a;
	enddef
endclass

class cat(animal)
	def speak(a)
		print "Meow" + a;
	enddef
endclass

class dog(animal)
	def speak(a)
		print "Woof" + a;
	enddef
endclass

c = new(cat)
d = new(dog)

c.speak("!")
d.speak("!")
