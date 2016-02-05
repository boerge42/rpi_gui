# **********************************************
#  weather_forecast_html.tcl
#  =========================
#       Uwe Berger; 2016
#
#  ---------
#  Have fun!
#
#
# **********************************************


# Dateinamen
source /home/bergeruw/work/weather_forecast/conf_fname.tcl

# ...fuer HTML-Zeugs
set pic_path	pics_forecast/png/
#set pic_dim     "width=\"120\" height=\"82\""
set pic_dim     ""

# **********************************************
proc getconf {fname} {
	set fd [open $fname r] 
	set data [read $fd [file size $fname]]
	close $fd
	return $data
}

# **********************************************
# **********************************************
# **********************************************

# einige Variablen koennten in der .conf-Datei fehlen,
# deshalb diese schon mal vordefinieren
set forecast4_day 		""
set forecast4_date 		""
set forecast4_icon 		""
set forecast4_temp_low 	""
set forecast4_temp_high ""
set forecast4_text 		""
set sunrise				""
set sunset				""

# Tcl-Variablen aus .conf-Datei erzeugen
set d [split [getconf  $conf_fname] \n]
foreach s $d {
	set s [split $s =]
	set key [string trim [lindex $s 0]]
	set value [string trim [lindex $s 1]]
	if {[string length $key]&&[string length $value]} {
		# puts "set $key $value"
		eval "set $key $value"
	}
}

# html-Tabelle generieren
set fd [open $html_fname w]
puts $fd "<table cellpadding=5>"

puts $fd "<tr>"
puts $fd "<td align=center colspan=5>"
puts $fd "<b>$city</b>"
puts $fd "<br><font size=2>(long./lat.: $longitude/$latitude; $current_date; sun: $sunrise/$sunset)</font>"
puts $fd "</td>"
puts $fd "</tr>"

puts $fd "<tr align=center>"
puts $fd "<td>$forecast0_day<br>$forecast0_date</td>"
puts $fd "<td>$forecast1_day<br>$forecast1_date</td>"
puts $fd "<td>$forecast2_day<br>$forecast2_date</td>"
puts $fd "<td>$forecast3_day<br>$forecast3_date</td>"
puts $fd "<td>$forecast4_day<br>$forecast4_date</td>"
puts $fd "</tr>"

puts $fd "<tr align=center>"
puts $fd "<td><img src=\"$pic_path$forecast0_icon\" $pic_dim></td>"
puts $fd "<td><img src=\"$pic_path$forecast1_icon\" $pic_dim></td>"
puts $fd "<td><img src=\"$pic_path$forecast2_icon\" $pic_dim></td>"
puts $fd "<td><img src=\"$pic_path$forecast3_icon\" $pic_dim></td>"
puts $fd "<td><img src=\"$pic_path$forecast4_icon\" $pic_dim></td>"
puts $fd "</tr>"

puts $fd "<tr align=center>"
puts $fd "<td>$forecast0_temp_low&deg;C<br>$forecast0_temp_high&deg;C</td>"
puts $fd "<td>$forecast1_temp_low&deg;C<br>$forecast1_temp_high&deg;C</td>"
puts $fd "<td>$forecast2_temp_low&deg;C<br>$forecast2_temp_high&deg;C</td>"
puts $fd "<td>$forecast3_temp_low&deg;C<br>$forecast3_temp_high&deg;C</td>"
puts $fd "<td>$forecast4_temp_low&deg;C<br>$forecast4_temp_high&deg;C</td>"
puts $fd "</tr>"

puts $fd "<tr align=center>"
puts $fd "<td width=150>$forecast0_text</td>"
puts $fd "<td width=150>$forecast1_text</td>"
puts $fd "<td width=150>$forecast2_text</td>"
puts $fd "<td width=150>$forecast3_text</td>"
puts $fd "<td width=150>$forecast4_text</td>"
puts $fd "</tr>"

puts $fd "<tr>"
puts $fd "<td align=right colspan=5>"
puts $fd "<font size=1>data from <a href=\"http://$source\">$source</a></font>"
puts $fd "</td>"
puts $fd "</tr>"

puts $fd "</table>"
close $fd
