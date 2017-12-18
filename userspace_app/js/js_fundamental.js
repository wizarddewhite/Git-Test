/* js fundamental */

/* 1. basic data type */

/* 1.1 integer */
var age = 18;
console.log('age is %d', age);
console.log('age is %d', age + 2);

/* 1.2 string */
// https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/Global_Objects/String
var name = 'xiaoming';
console.log('name is %s', name);
console.log('name is %s', name + ' zhang');

/* 1.3 boolean */
var isAdult = false;

if (age >= 18)
  isAdult = true;
else
  isAdult = false;

console.log('%s %s', name, isAdult?'is Adult':'isn\'t Adult');

/* 1.4 list */
// https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/Global_Objects/Array
var myLikeColorsList = ['blue', 'green', 'black', 'white'];
console.log("myLikeColorsList has %d colors", myLikeColorsList.length);
console.log("\tFirst color is %s", myLikeColorsList[0]);
// add one to the end
myLikeColorsList.push('purple');
// delete one at the beginning
myLikeColorsList.splice(0,1);
for (color in myLikeColorsList)
  console.log("I like %s", myLikeColorsList[color]);

/* 1.5 define object */
var JsLessonInfo = {
  teacher: name,
  title: 'JavaScript编程入门',
  beginTime: '2018/8/8',
  endTime: '2018/9/4',
  Student: 417,
  lessionTarget: '帮助更多小白学习JavaScript',
  githubUrl: 'https://github.com/xugy0926/getting-started-with-javascript'
};
console.log(JsLessonInfo);
JsLessonInfo.teacher = name + ' zhang';
console.log("Teacher name: %s", JsLessonInfo.teacher);

// syntax suguar
var xiaoming = {
  name,
  age
}
console.log("%s is %d year's old", xiaoming.name, xiaoming.age);

/* 2. control flow */

/* 2.1 if */

if (age >= 80)
  console.log("A god");
else if (age >= 60)
  console.log("An elder");
else if (age >= 40)
  console.log("A strong");
else
  console.log("A child");

/* 2.2 switch */
switch(age)
{
case 0:
  console.log("new baby");
  break;
case 18:
  console.log("grown up");
  break;
default:
  console.log("living?")
}

/* 2.3 for */

for (var i = 0; i < myLikeColorsList.length; i++)
  console.log(myLikeColorsList[i])

/* 2.4 while */
var i = 0;
while (i < myLikeColorsList.length) {
  console.log(myLikeColorsList[i]);
  i++;
}

/* 3. function */

function sum_to(n)
{
  var sum = 0;
  var i = 0;
  for (i = 0; i <= n; i++) {
    sum += i;
  }
  return sum;
}

console.log(sum_to(10));
