project(1-2-ConsolProjects)

set(SRC
BankAccount/BankAccount.cpp
BankAccount/BankAccount.h
BankAccount/Main.cpp
)
add_executable(2-BankAccount ${SRC})

set(SRC
BinaryStream/Main.cpp
BinaryStream/Numbers.bin
)
add_executable(2-BinaryStream ${SRC})

set(SRC
Car/Car.cpp
Car/Car.h
Car/Main.cpp
)
add_executable(2-Car ${SRC})

set(SRC
Exception/Main.cpp
Exception/TemplateCell.cpp
Exception/TemplateCell.h
)
add_executable(2-Exception ${SRC})

set(SRC
HigherOrderFunctions/Main.cpp
)
add_executable(1-43-HighOrderFunction ${SRC})

set(SRC
Inheritance1/Employee.cpp
Inheritance1/Employee.h
Inheritance1/Main.cpp
Inheritance1/Person.cpp
Inheritance1/Person.h
Inheritance1/Student.cpp
Inheritance1/Student.h
)
add_executable(2-58-Inheritance1 ${SRC})

set(SRC
Inheritance2/Employee.cpp
Inheritance2/Employee.h
Inheritance2/Main.cpp
Inheritance2/Person.cpp
Inheritance2/Person.h
Inheritance2/Student.cpp
Inheritance2/Student.h
)
add_executable(2-61-Inheritance2 ${SRC})

set(SRC
LinkedList/Cell.cpp
LinkedList/Cell.h
LinkedList/Main.cpp
)
add_executable(2-66-LinkedList ${SRC})

set(SRC
Rational/Main.cpp
Rational/Rational.cpp
Rational/Rational.h
)
add_executable(2-70-Rational ${SRC})

set(SRC
Size/Main.cpp
)
add_executable(1-19-Size ${SRC})

set(SRC
Stack/Main.cpp
Stack/Stack.cpp
Stack/Stack.h
)
add_executable(2-66-Stack ${SRC})

set(SRC
TemplateStack/Main.cpp
TemplateStack/TemplateCell.h
TemplateStack/TemplateStack.h
)
add_executable(2-77-TemplateStack ${SRC})

set(SRC
TextStream/Input.txt
TextStream/Main.cpp
TextStream/Output.txt
)
add_executable(2-82-TextStream ${SRC})
