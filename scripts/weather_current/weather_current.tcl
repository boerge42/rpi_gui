# **********************************************
#  weather_current.tcl
#  ===================
#   Uwe Berger; 2015
#
#  Holt die aktuellen Wetterdaten aus meiner 
#  Wetter-Datenbank und speichert sie im
#  libconfig-Format ab.
#
#  ---------
#  Have fun!
#
# **********************************************

package require sqlite3

set wetter_db	/media/fritzbox/WD-MyPassport0810-01/dockstar/wetter_db/weather_log_db
set altitude	47.5
set conf_fname	/home/bergeruw/public_html/temp/weather_current.conf
#set conf_fname	../weather_current.conf

 
# **********************************************
# **********************************************
# **********************************************
sqlite3 db $wetter_db

db timeout 20000

set fd [open $conf_fname w]

# Timestamp
#puts $fd "timestamp=\"[clock format [clock seconds] -format "%d.%m.%Y, %H:%M:%S"]\""
set sql "select max(ts) from temp_log"
set r [db eval $sql]
puts $fd "timestamp=\"[clock format [clock scan [lindex $r 0] -format "%Y-%m-%dT%H:%M:%S"] -format "%d.%m.%Y, %H:%M:%S"]\""

# Temperaturen
set sql "select sht15_temp, temp_log.'107D6EB501080057'\
		 from temp_log \
		 where ts = (select max(ts) from temp_log)"
#set sql "select sht15_temp, bmp085_temp from last_values where tab = 'temp_log'"
set r [db eval $sql]
puts $fd "temperature_out=\"[format "%.1f" [lindex $r 0]]\""
puts $fd "temperature_in=\"[ format "%.1f" [lindex $r 1]]\""

# Luftfeuchtigkeiten
set sql "select hih4030_hum, sht15_hum \
		 from view_tab_humidity \
		 where ts = (select max(ts) from view_tab_humidity)"
#set sql "select hih4030_hum, sht15_hum from last_values where tab = 'humidity_log'"
set r [db eval $sql]
puts $fd "humidity_in=\"[format "%.1f" [lindex $r 0]]\""
puts $fd "humidity_out=\"[format "%.1f" [lindex $r 1]]\""

# Luftdruck
set sql "select pressure, pressure+$altitude/8.0  \
		 from pressure_log \
		 where ts = (select max(ts) from pressure_log)"
#set sql "select pressure, pressure+$altitude/8.0 from last_values where tab = 'pressure_log'"
set r [db eval $sql]
set pressure_abs [lindex $r 0]
puts $fd "pressure_rel=\"[format "%.1f" [lindex $r 1]]\""

# Tendenz Luftdruck
#set sql "select pressure, pressure+$altitude/8.0  \
#		 from pressure_log \
#		 where ts = (select max(ts) from pressure_log)"
	 
set sql "select avg(pressure)\
			 from pressure_log \
			 where ts >= strftime('%Y-%m-%dT%H:%M:%S', 'now', '-3 hour')"		 
# set sql "select pressure from last_values where tab = 'pressure_3h_avg'"
set r [db eval $sql]
if {$pressure_abs < [lindex $r 0]} {
	puts $fd "pressure_rising=0"
} else {
	puts $fd "pressure_rising=1"	
}

close $fd
db close
