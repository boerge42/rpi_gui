# **********************************************
#  weather_tuxnet24_openweathermap.tcl
#  ===================================
#         Uwe Berger; 2016
#
#  ---------
#  Have fun!
#
#
# **********************************************

package require http

# Request-URL zusammenbauen
set zip         zip=14772,DE
set mapid       mapid=2945358
set apikey      apikey=xyz
set unit        unit=c
set language    language=en
set mode        mode=txt
set q		q=Brandenburg_an_der_Havel
set lon		lon=12.55
set lat		lat=52.42

# set url	http://weather.tuxnet24.de/?$apikey&$mapid&$unit&$language&$mode
#set url	http://weather.tuxnet24.de/?$apikey&$q&$unit&$language&$mode
set url		http://weather.tuxnet24.de/?$apikey&$lon&$lat&$unit&$language&$mode

# Dateiname fuer Outputs
source /home/bergeruw/work/weather_forecast/conf_fname.tcl


# **********************************************
proc getpage { url } {
	set token [::http::geturl $url]
	set data [::http::data $token]
	::http::cleanup $token          
	return $data
}

# **********************************************
proc ___getpage {url} {
	set fname data/tuxnet24_openweathermap.txt
	set fd [open $fname r] 
	set data [read $fd [file size $fname]]
	close $fd
	return $data
}

# **********************************************
proc format_icon_name {iname} {
	# meine Icons haben kein "-" im Dateinamen
	set iname [string map {- ""} $iname]
	# wenn an 4.Stelle ein ".", dann davor noch ein "d" einfuegen
	if {[string match . [string index $iname 3]]} {
		set iname [string map {. d.} $iname]
	}
	return $iname
}

# **********************************************
proc eleminate_degree {value} {
	return [string map {°C ""} $value]
}

# **********************************************
proc eleminate_clock {value} {
	return [string map {" clock" ""} $value]
}
 
# **********************************************
proc convert_current_date {d} {
	return [clock format [clock scan $d] -format "%a, %d.%m.%Y, %H:%M"]
}

# **********************************************
proc convert_date {d} {
	return [clock format [clock scan $d] -format "%d.%m.%Y"]
}
 
# **********************************************
proc convert_day {d} {
	return [clock format [clock scan $d] -format "%a"]
}
 
# **********************************************
# **********************************************
# **********************************************


set d [split [getpage  $url] \n]

# libconf-Fomat generieren
set fd [open $conf_fname w]
foreach s $d {
	set s [split $s =]
	set key [string trim [lindex $s 0]]
	set value [string trim [lindex $s 1]]
	if {[string length $key]&&[string length $value]} {
		# Werte ein wenig grundsaetzlich formatieren
		set value [eleminate_clock $value]
		set value [eleminate_degree $value]
		set value [string map {[ (} $value]
		set value [string map {] )} $value]
		set value [string trim $value " "]
		# Datumangaben formatieren
		if {[string first current_date $key] > -1} {
			set value [convert_current_date $value]
		} else {
			if {[string first _date $key] > -1} {set value [convert_date $value]}
			if {[string first _day $key] > -1} {set value [convert_day $value]}
		}		
		# bei ..._image .png durch .jpg austauschen
		if {[string first image $key] > -1} {set value [string map {".png" ".jpg"} $value]}
		# bei ..._icon etwas umformatieren
		if {[string first icon $key] > -1} {set value [format_icon_name $value]}
		# conf-Datei schreiben
		puts $fd "$key = \"$value\""
	}
}
# Datenquelle vermerken
puts $fd "source = \"weather.tuxnet24.de\""
close $fd

# html-Datei erzeugen
source /home/bergeruw/work/weather_forecast/weather_forecast_html.tcl
