cmd_/home/orochy/Practica/day5/pcidev.mod := printf '%s\n'   pcidev.o | awk '!x[$$0]++ { print("/home/orochy/Practica/day5/"$$0) }' > /home/orochy/Practica/day5/pcidev.mod
