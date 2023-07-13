cmd_/home/orochy/Practica/day2/dev1.mod := printf '%s\n'   dev1.o | awk '!x[$$0]++ { print("/home/orochy/Practica/day2/"$$0) }' > /home/orochy/Practica/day2/dev1.mod
