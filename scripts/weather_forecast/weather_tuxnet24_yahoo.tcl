# **********************************************
#  weather_tuxnet24_yahoo.tcl
#  ==========================
#      Uwe Berger; 2016
#
#  ---------
#  Have fun!
#
# **********************************************

package require http

set city_code	GMBB0137
set url			http://weather.tuxnet24.de/?id=$city_code

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
proc ____getpage {url} {
	set fname data/tuxnet24_yahoo.txt
	set fd [open $fname r] 
	set data [read $fd [file size $fname]]
	close $fd
	return $data
}

# **********************************************
proc eleminate_degree {value} {
	return [string map {°C ""} $value]
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
proc convert_time {d} {
	return [clock format [clock scan $d] -format "%H:%M"]
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
	if {[string length $key]&&[string length $value]&&([string first image $key]==-1)} {
		# Grad Celsius weg
		set value [eleminate_degree $value]
		# Datumangaben formatieren
		if {[string first current_date $key] > -1} {
			set value [convert_current_date $value]
		} else {
			if {[string first _date $key] > -1} {set value [convert_date $value]}
			if {[string first sunrise $key] > -1} {set value [convert_time $value]}
			if {[string first sunset $key] > -1} {set value [convert_time $value]}			
		}
		# Leerzeichen am Anfang/Ende weg
		set value [string trim $value " "]
		puts $fd "$key = \"$value\""
		# aus forecastx_code _icon und _image ermitteln/schreiben
		if {[string first code $key] > -1} {
			set k [string map {_code ""} $key]
			puts $fd "$k\_image = \"$value\.jpg\""
			puts $fd "$k\_icon = \"$value\.png\""
		}
	}
}
# Datenquelle vermerken
puts $fd "source = \"weather.tuxnet24.de\""
close $fd

# html-Datei erzeugen
source /home/bergeruw/work/weather_forecast/weather_forecast_html.tcl

