cmd_/home/orochy/Practica/day1/dev1.mod := printf '%s\n'   dev1.o | awk '!x[$$0]++ { print("/home/orochy/Practica/day1/"$$0) }' > /home/orochy/Practica/day1/dev1.mod
