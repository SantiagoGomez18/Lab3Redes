msgs=(
"Minuto 0: ¡Comienza el partido entre Equipo A y Equipo B!"
"Minuto 4: Equipo A presiona en el área rival"
"Minuto 7: Primera falta de Equipo B sobre jugador 9 de Equipo A"
"Minuto 10: Remate desviado de jugador 7 de Equipo A"
"Minuto 15: Gol de Equipo B, jugador 10"
"Minuto 20: Equipo A busca reaccionar, domina el balón"
"Minuto 23: Tarjeta amarilla al jugador 4 de Equipo A"
"Minuto 27: Disparo de fuera del área de jugador 8 de Equipo B, el portero ataja"
"Minuto 30: Corner para Equipo A"
"Minuto 33: Gol de Equipo A, jugador 9, cabezazo tras corner"
"Minuto 38: Falta peligrosa de jugador 6 de Equipo B"
"Minuto 42: Tiro libre directo, el balón pasa rozando el palo"
"Minuto 45+1: Oportunidad de gol fallida por jugador 11 de Equipo A"
"Minuto 46: Comienza el segundo tiempo"
"Minuto 53: Gol de Equipo B, jugador 7, remate desde fuera del área"
"Minuto 60: Cambio, jugador 12 entra por jugador 6 de Equipo A"
"Minuto 72: Tarjeta roja al jugador 3 de Equipo B"
"Minuto 81: Gol de Equipo A, jugador 10, tras un contraataque rápido"
"Minuto 90+3: Final del partido, marcador final 3-2 a favor de Equipo A"
)

for msg in "${msgs[@]}"; do
  printf 'Futbol|%s\n' "$msg" | nc -u -w1 127.0.0.1 6000
done
