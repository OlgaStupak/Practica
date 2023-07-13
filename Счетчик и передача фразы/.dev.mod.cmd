cmd_/home/orochy/Practica/day1/dev.mod := printf '%s\n'   dev.o | awk '!x[$$0]++ { print("/home/orochy/Practica/day1/"$$0) }' > /home/orochy/Practica/day1/dev.mod
