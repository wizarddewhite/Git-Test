#!/usr/bin/python
# Filename: inherit.py

class SchoolMember(object):
    '''Represents any school member.'''
    def __init__(self, name, age):
        self.name = name
        self.age = age
        print '(Initialized SchoolMember: %s)' % self.name
    def tell(self):
        '''Tell my details.'''
        print 'Name:"%s" Age:"%s"' %(self.name, self.age)
    def say(self, sentence):
        print sentence
class Teacher(SchoolMember):
    '''Represents a teacher.'''
    def __init__(self, name, age, salary):
        super(Teacher, self).__init__(name, age)
        self.salary = salary
        print '(Initialized Teacher: %s)' %self.name
    def tell(self):
        SchoolMember.tell(self)
        print 'Salary: "%d"' % self.salary
class Student(SchoolMember):
    '''Represents a Student.'''
    def __init__(self, name, age, marks):
        super(Student, self).__init__(name, age)
        self.marks = marks
        print '(Initialized Student: %s)' %self.name
    def tell(self):
        SchoolMember.tell(self)
        print 'marks: "%d"' % self.marks
    def say(self, sentence, mood):
        print "%s, I am %s" %(sentence, mood)
#    def say(self, sentence):
#        SchoolMember.say(self, sentence)
t = Teacher("Mrs. Shrividya", 40, 3000)
s = Student('Swaroop', 22, 75) 
t.tell()
s.say("Good morning", "happy")
#s.say("Good Bye")
