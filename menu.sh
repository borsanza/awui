#!/bin/bash
# Menú para compilar y lanzar awui: Linux (Release, Debug, sanitizers), Windows (desde Linux, con Docker) y la
# versión de Windows con Wine. Usa whiptail si está; si no, un menú de texto.
#
#   ./menu.sh                    menú
#   ./menu.sh <acción> [...]     sin menú, por ejemplo:
#       ./menu.sh build release          (release, debug, sanitize, windows, installer, deb o all)
#       ./menu.sh run release stationTV  (release, debug o sanitize)
#       ./menu.sh wine stationTV
#       ./menu.sh clean
set -uo pipefail

cd "$(dirname "$0")"
ROOT=$PWD
STATE="${XDG_CACHE_HOME:-$HOME/.cache}/awui/menu"
mkdir -p "$STATE"

if command -v whiptail > /dev/null && [ -t 0 ] && [ -t 1 ]; then
	UI=whiptail
else
	UI=text
fi

# Carpeta de cada compilación de Linux (los presets de CMakePresets.json)
build_dir() {
	case "$1" in
		release) echo build ;;
		debug) echo build-debug ;;
		sanitize) echo build-sanitize ;;
	esac
}

samples() {
	local dir
	for dir in samples/*/; do
		[ -f "$dir/CMakeLists.txt" ] && basename "$dir"
	done
}

remember() { echo "$2" > "$STATE/$1"; }
recall() { cat "$STATE/$1" 2>/dev/null || echo "$2"; }

# ------------------------------------------------------------------------------------------------- interfaz

# choose <título> <texto> <por defecto> <etiqueta> <descripción> ... : escribe la etiqueta elegida (vacío si cancela)
choose() {
	local title=$1 text=$2 default=$3
	shift 3
	if [ $UI = whiptail ]; then
		local count=$(($# / 2))
		whiptail --title "$title" --default-item "$default" --menu "$text" $((count + 9)) 72 "$count" "$@" \
			3>&1 1>&2 2>&3
		return
	fi

	echo >&2
	echo "== $title" >&2
	[ -n "$text" ] && echo "$text" >&2
	local tags=() i=1
	while [ $# -gt 0 ]; do
		tags+=("$1")
		printf '  %2d) %-16s %s\n' "$i" "$1" "$2" >&2
		shift 2
		i=$((i + 1))
	done
	local answer
	read -r -p "Elige (Enter: $default, q: volver): " answer >&2 || return
	if [ -z "$answer" ]; then
		echo "$default"
	elif [[ "$answer" =~ ^[0-9]+$ ]] && [ "$answer" -ge 1 ] && [ "$answer" -le ${#tags[@]} ]; then
		echo "${tags[$((answer - 1))]}"
	fi
}

pause() {
	echo
	read -r -p "Pulsa Enter para volver al menú..." _ || true
}

# Ejecuta una acción mostrando su salida y avisa de cómo ha ido
run_step() {
	local description=$1
	shift
	echo
	echo "==> $description"
	if "$@"; then
		echo "==> Hecho: $description"
		return 0
	fi
	echo "==> ERROR: $description" >&2
	return 1
}

# ---------------------------------------------------------------------------------------------- acciones

do_build() {
	local what=$1
	case "$what" in
		release | debug | sanitize)
			run_step "Compilar Linux ($what)" \
				bash -c "cmake --preset $what > /dev/null && cmake --build --preset $what"
			;;
		windows)
			run_step "Compilar Windows" scripts/build-windows.sh
			;;
		installer)
			run_step "Generar el instalador de StationTV para Windows" scripts/build-windows.sh --installer
			;;
		deb)
			run_step "Generar el paquete .deb de StationTV" scripts/build-deb.sh
			;;
		all)
			do_build release && do_build debug && do_build sanitize && do_build installer && do_build deb
			;;
		*)
			echo "Compilación desconocida: $what" >&2
			return 1
			;;
	esac
}

# Lanza un sample de Linux desde su carpeta (carga images/, roms/, lang/... con rutas relativas). Las imágenes y ROMs
# están versionadas en build/samples/: en las otras compilaciones se enlazan la primera vez
do_run() {
	local what=$1 sample=$2 dir
	dir=$(build_dir "$what")
	local exe="$dir/samples/$sample/$sample"
	if [ ! -x "$exe" ]; then
		echo "No está $exe: compílalo antes (./menu.sh build $what)" >&2
		return 1
	fi

	local assets
	for assets in images roms; do
		if [ "$dir" != build ] && [ -d "build/samples/$sample/$assets" ] && [ ! -e "$dir/samples/$sample/$assets" ]; then
			ln -s "$ROOT/build/samples/$sample/$assets" "$dir/samples/$sample/$assets"
		fi
	done

	echo "==> $sample ($what)"
	(
		cd "$dir/samples/$sample" || exit 1
		if [ "$what" = sanitize ]; then
			ASAN_OPTIONS=${ASAN_OPTIONS:-detect_leaks=0} "./$sample"
		else
			"./$sample"
		fi
	)
}

do_wine() {
	run_step "Lanzar $1 con Wine" scripts/run-windows.sh "$1"
}

do_clean() {
	local dir
	for dir in build-debug build-sanitize build-windows build-package build-package-*; do
		[ -d "$dir" ] && rm -rf "$dir" && echo "Borrado $dir/"
	done
	# En build/ están las imágenes y ROMs versionadas: solo se borra lo compilado
	if [ -f build/CMakeCache.txt ]; then
		cmake --build build --target clean > /dev/null 2>&1
		rm -rf build/CMakeCache.txt build/CMakeFiles
		echo "Limpiado build/ (sin tocar imágenes ni ROMs)"
	fi
}

# --------------------------------------------------------------------------------------------- submenús

pick_sample() {
	local args=() sample
	for sample in $(samples); do
		args+=("$sample" "")
	done
	choose "Sample" "¿Qué programa?" "$(recall sample stationTV)" "${args[@]}"
}

pick_linux_build() {
	local args=() what
	for what in release debug sanitize; do
		if [ -d "$(build_dir "$what")/samples" ]; then
			args+=("$what" "$(build_dir "$what")/")
		else
			args+=("$what" "(sin compilar)")
		fi
	done
	choose "Compilación" "¿Cuál?" "$(recall linux-build release)" "${args[@]}"
}

menu_build() {
	local what
	what=$(choose "Compilar" "" "$(recall build release)" \
		release "Linux, Release (build/)" \
		debug "Linux, Debug (build-debug/)" \
		sanitize "Linux, ASan + UBSan (build-sanitize/)" \
		windows "Windows, MinGW con Docker (build-windows/)" \
		installer "Instalador de StationTV para Windows (build-windows/)" \
		deb "Paquete .deb de StationTV, con Docker (build-package/)" \
		all "Linux, Windows, el instalador y el paquete")
	[ -z "$what" ] && return
	remember build "$what"
	do_build "$what"
	pause
}

menu_run_linux() {
	local what sample
	what=$(pick_linux_build)
	[ -z "$what" ] && return
	sample=$(pick_sample)
	[ -z "$sample" ] && return
	remember linux-build "$what"
	remember sample "$sample"
	if [ ! -x "$(build_dir "$what")/samples/$sample/$sample" ]; then
		do_build "$what" || { pause; return; }
	fi
	do_run "$what" "$sample"
	pause
}

menu_run_wine() {
	local sample
	sample=$(pick_sample)
	[ -z "$sample" ] && return
	remember sample "$sample"
	if [ ! -f "build-windows/samples/$sample/$sample.exe" ]; then
		do_build windows || { pause; return; }
	fi
	do_wine "$sample"
	pause
}

main_menu() {
	local action
	while true; do
		action=$(choose "awui" "Compilar y lanzar" "$(recall action build)" \
			build "Compilar..." \
			run "Lanzar en Linux..." \
			wine "Lanzar la versión de Windows con Wine..." \
			clean "Borrar lo compilado (no toca imágenes ni ROMs)" \
			quit "Salir")
		case "$action" in
			"" | quit) break ;;
		esac
		remember action "$action"
		case "$action" in
			build) menu_build ;;
			run) menu_run_linux ;;
			wine) menu_run_wine ;;
			clean)
				if [ $UI = whiptail ]; then
					whiptail --title "Borrar" --yesno "¿Borrar build-debug/, build-sanitize/, build-windows/, build-package/ y lo compilado en build/?" 9 72 \
						&& { do_clean; pause; }
				else
					read -r -p "¿Borrar todo lo compilado? (s/N) " answer
					[ "$answer" = s ] && { do_clean; pause; }
				fi
				;;
		esac
	done
}

# ------------------------------------------------------------------------------------------------ inicio

if [ $# -eq 0 ]; then
	# Ctrl+C para el programa que se esté ejecutando (compilación, juego...) y se vuelve al menú. Un trap con orden (no
	# vacío) no lo heredan los hijos: a ellos les llega la señal normal
	trap ':' INT
	main_menu
	exit 0
fi

action=$1
shift
case "$action" in
	build) do_build "${1:-release}" ;;
	run) do_run "${1:-release}" "${2:-stationTV}" ;;
	wine) do_wine "${1:-stationTV}" ;;
	clean) do_clean ;;
	*)
		sed -n '2,10p' "$0" | sed 's/^# \{0,1\}//'
		exit 1
		;;
esac
