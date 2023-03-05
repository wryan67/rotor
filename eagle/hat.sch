<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.6.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="raspberry_pi_3b">
<packages>
<package name="MODULE_RASPBERRY_PI_3B+_HDR">
<text x="-1.348" y="-3.83" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<text x="-1.221" y="4.997" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<pad name="1" x="0.033" y="-0.011" drill="1.016" shape="square"/>
<pad name="3" x="2.573" y="-0.011" drill="1.016"/>
<pad name="5" x="5.113" y="-0.011" drill="1.016"/>
<pad name="7" x="7.653" y="-0.011" drill="1.016"/>
<pad name="9" x="10.193" y="-0.011" drill="1.016"/>
<pad name="11" x="12.733" y="-0.011" drill="1.016"/>
<pad name="2" x="0.033" y="2.529" drill="1.016"/>
<pad name="4" x="2.573" y="2.529" drill="1.016"/>
<pad name="6" x="5.113" y="2.529" drill="1.016"/>
<pad name="8" x="7.653" y="2.529" drill="1.016"/>
<pad name="10" x="10.193" y="2.529" drill="1.016"/>
<pad name="12" x="12.733" y="2.529" drill="1.016"/>
<pad name="13" x="15.273" y="-0.011" drill="1.016"/>
<pad name="14" x="15.273" y="2.529" drill="1.016"/>
<pad name="15" x="17.813" y="-0.011" drill="1.016"/>
<pad name="17" x="20.353" y="-0.011" drill="1.016"/>
<pad name="19" x="22.893" y="-0.011" drill="1.016"/>
<pad name="21" x="25.433" y="-0.011" drill="1.016"/>
<pad name="23" x="27.973" y="-0.011" drill="1.016"/>
<pad name="25" x="30.513" y="-0.011" drill="1.016"/>
<pad name="16" x="17.813" y="2.529" drill="1.016"/>
<pad name="18" x="20.353" y="2.529" drill="1.016"/>
<pad name="20" x="22.893" y="2.529" drill="1.016"/>
<pad name="22" x="25.433" y="2.529" drill="1.016"/>
<pad name="24" x="27.973" y="2.529" drill="1.016"/>
<pad name="26" x="30.513" y="2.529" drill="1.016"/>
<pad name="27" x="33.053" y="-0.011" drill="1.016"/>
<pad name="28" x="33.053" y="2.529" drill="1.016"/>
<pad name="29" x="35.593" y="-0.011" drill="1.016"/>
<pad name="31" x="38.133" y="-0.011" drill="1.016"/>
<pad name="33" x="40.673" y="-0.011" drill="1.016"/>
<pad name="35" x="43.213" y="-0.011" drill="1.016"/>
<pad name="37" x="45.753" y="-0.011" drill="1.016"/>
<pad name="39" x="48.293" y="-0.011" drill="1.016"/>
<pad name="30" x="35.593" y="2.529" drill="1.016"/>
<pad name="32" x="38.133" y="2.529" drill="1.016"/>
<pad name="34" x="40.673" y="2.529" drill="1.016"/>
<pad name="36" x="43.213" y="2.529" drill="1.016"/>
<pad name="38" x="45.753" y="2.529" drill="1.016"/>
<pad name="40" x="48.293" y="2.529" drill="1.016"/>
<wire x1="-1.905" y1="-1.905" x2="-1.905" y2="4.445" width="0.127" layer="39"/>
<wire x1="-1.905" y1="4.445" x2="50.165" y2="4.445" width="0.127" layer="39"/>
<wire x1="50.165" y1="4.445" x2="50.165" y2="-1.905" width="0.127" layer="39"/>
<wire x1="50.165" y1="-1.905" x2="-1.905" y2="-1.905" width="0.127" layer="39"/>
<wire x1="-1.27" y1="3.81" x2="49.53" y2="3.81" width="0.127" layer="21"/>
<wire x1="49.53" y1="3.81" x2="49.53" y2="-1.27" width="0.127" layer="21"/>
<wire x1="49.53" y1="-1.27" x2="-1.27" y2="-1.27" width="0.127" layer="21"/>
<wire x1="-1.27" y1="-1.27" x2="-1.27" y2="3.81" width="0.127" layer="21"/>
</package>
</packages>
<symbols>
<symbol name="RASPBERRY_PI_3B+_HDR">
<wire x1="-20.32" y1="33.02" x2="20.32" y2="33.02" width="0.254" layer="94"/>
<wire x1="20.32" y1="33.02" x2="20.32" y2="-30.48" width="0.254" layer="94"/>
<wire x1="20.32" y1="-30.48" x2="-20.32" y2="-30.48" width="0.254" layer="94"/>
<wire x1="-20.32" y1="-30.48" x2="-20.32" y2="33.02" width="0.254" layer="94"/>
<text x="-20.32" y="33.528" size="2.54" layer="95">&gt;NAME</text>
<text x="-20.32" y="-33.02" size="2.54" layer="96">&gt;VALUE</text>
<pin name="5V" x="25.4" y="30.48" length="middle" direction="pwr" rot="R180"/>
<pin name="3V3" x="25.4" y="27.94" length="middle" direction="pwr" rot="R180"/>
<pin name="GPIO2/SDA1" x="-25.4" y="25.4" length="middle"/>
<pin name="GPIO3/SCL1" x="-25.4" y="22.86" length="middle"/>
<pin name="GPIO4/GPIO_GCKL" x="-25.4" y="20.32" length="middle"/>
<pin name="GPIO14/TXD0" x="25.4" y="20.32" length="middle" rot="R180"/>
<pin name="GND" x="25.4" y="-27.94" length="middle" direction="pwr" rot="R180"/>
<pin name="GPIO15/RXD0" x="25.4" y="17.78" length="middle" rot="R180"/>
<pin name="GPIO17/GPIO_GEN0" x="-25.4" y="15.24" length="middle"/>
<pin name="GPIO18/GPIO_GEN1" x="-25.4" y="10.16" length="middle"/>
<pin name="GPIO27/GPIO_GEN2" x="-25.4" y="12.7" length="middle"/>
<pin name="GPIO22/GPIO_GEN3" x="-25.4" y="7.62" length="middle"/>
<pin name="GPIO23/GPIO_GEN4" x="-25.4" y="5.08" length="middle"/>
<pin name="GPIO24/GPIO_GEN5" x="-25.4" y="2.54" length="middle"/>
<pin name="GPIO10/SPI_MOSI" x="-25.4" y="-12.7" length="middle"/>
<pin name="GPIO9/SPI_MISO" x="-25.4" y="-15.24" length="middle"/>
<pin name="GPIO25/GPIO_GEN6" x="-25.4" y="0" length="middle"/>
<pin name="GPIO11/SPI_SCLK" x="-25.4" y="-10.16" length="middle"/>
<pin name="GPIO8/!SPI_CE0!" x="-25.4" y="-5.08" length="middle"/>
<pin name="GPIO7/!SPI_CE1!" x="-25.4" y="-7.62" length="middle"/>
<pin name="ID_SD" x="-25.4" y="-20.32" length="middle"/>
<pin name="ID_SC" x="-25.4" y="-22.86" length="middle"/>
<pin name="GPIO5" x="25.4" y="12.7" length="middle" rot="R180"/>
<pin name="GPIO6" x="25.4" y="10.16" length="middle" rot="R180"/>
<pin name="GPIO12" x="25.4" y="7.62" length="middle" rot="R180"/>
<pin name="GPIO13" x="25.4" y="5.08" length="middle" rot="R180"/>
<pin name="GPIO19" x="25.4" y="0" length="middle" rot="R180"/>
<pin name="GPIO16" x="25.4" y="2.54" length="middle" rot="R180"/>
<pin name="GPIO26" x="25.4" y="-7.62" length="middle" rot="R180"/>
<pin name="GPIO20" x="25.4" y="-2.54" length="middle" rot="R180"/>
<pin name="GPIO21" x="25.4" y="-5.08" length="middle" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="RASPBERRY_PI_3B+_HDR" prefix="U">
<description> &lt;a href="https://pricing.snapeda.com/parts/RASPBERRY%20PI%203%20MODEL%20B%2B/Raspberry%20Pi/view-part?ref=eda"&gt;Check availability&lt;/a&gt;</description>
<gates>
<gate name="G$1" symbol="RASPBERRY_PI_3B+_HDR" x="0" y="0"/>
</gates>
<devices>
<device name="MODEL_RASPBEERY_PI_3B_HDR" package="MODULE_RASPBERRY_PI_3B+_HDR">
<connects>
<connect gate="G$1" pin="3V3" pad="1 17"/>
<connect gate="G$1" pin="5V" pad="2 4"/>
<connect gate="G$1" pin="GND" pad="6 9 14 20 25 30 34 39"/>
<connect gate="G$1" pin="GPIO10/SPI_MOSI" pad="19"/>
<connect gate="G$1" pin="GPIO11/SPI_SCLK" pad="23"/>
<connect gate="G$1" pin="GPIO12" pad="32"/>
<connect gate="G$1" pin="GPIO13" pad="33"/>
<connect gate="G$1" pin="GPIO14/TXD0" pad="8"/>
<connect gate="G$1" pin="GPIO15/RXD0" pad="10"/>
<connect gate="G$1" pin="GPIO16" pad="36"/>
<connect gate="G$1" pin="GPIO17/GPIO_GEN0" pad="11"/>
<connect gate="G$1" pin="GPIO18/GPIO_GEN1" pad="12"/>
<connect gate="G$1" pin="GPIO19" pad="35"/>
<connect gate="G$1" pin="GPIO2/SDA1" pad="3"/>
<connect gate="G$1" pin="GPIO20" pad="38"/>
<connect gate="G$1" pin="GPIO21" pad="40"/>
<connect gate="G$1" pin="GPIO22/GPIO_GEN3" pad="15"/>
<connect gate="G$1" pin="GPIO23/GPIO_GEN4" pad="16"/>
<connect gate="G$1" pin="GPIO24/GPIO_GEN5" pad="18"/>
<connect gate="G$1" pin="GPIO25/GPIO_GEN6" pad="22"/>
<connect gate="G$1" pin="GPIO26" pad="37"/>
<connect gate="G$1" pin="GPIO27/GPIO_GEN2" pad="13"/>
<connect gate="G$1" pin="GPIO3/SCL1" pad="5"/>
<connect gate="G$1" pin="GPIO4/GPIO_GCKL" pad="7"/>
<connect gate="G$1" pin="GPIO5" pad="29"/>
<connect gate="G$1" pin="GPIO6" pad="31"/>
<connect gate="G$1" pin="GPIO7/!SPI_CE1!" pad="26"/>
<connect gate="G$1" pin="GPIO8/!SPI_CE0!" pad="24"/>
<connect gate="G$1" pin="GPIO9/SPI_MISO" pad="21"/>
<connect gate="G$1" pin="ID_SC" pad="28"/>
<connect gate="G$1" pin="ID_SD" pad="27"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="pinhead" urn="urn:adsk.eagle:library:325">
<description>&lt;b&gt;Pin Header Connectors&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="2X13" urn="urn:adsk.eagle:footprint:22283/1" library_version="5">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-16.51" y1="-1.905" x2="-15.875" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-15.875" y1="-2.54" x2="-14.605" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-14.605" y1="-2.54" x2="-13.97" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-13.97" y1="-1.905" x2="-13.335" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-13.335" y1="-2.54" x2="-12.065" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-12.065" y1="-2.54" x2="-11.43" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-11.43" y1="-1.905" x2="-10.795" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-10.795" y1="-2.54" x2="-9.525" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-9.525" y1="-2.54" x2="-8.89" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="-1.905" x2="-8.255" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-8.255" y1="-2.54" x2="-6.985" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-6.985" y1="-2.54" x2="-6.35" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="-1.905" x2="-5.715" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-5.715" y1="-2.54" x2="-4.445" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="-2.54" x2="-3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="-1.905" x2="-3.175" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="-2.54" x2="-1.905" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="-2.54" x2="-1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-16.51" y1="-1.905" x2="-16.51" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-16.51" y1="1.905" x2="-15.875" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-15.875" y1="2.54" x2="-14.605" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-14.605" y1="2.54" x2="-13.97" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-13.97" y1="1.905" x2="-13.335" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-13.335" y1="2.54" x2="-12.065" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-12.065" y1="2.54" x2="-11.43" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-11.43" y1="1.905" x2="-10.795" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-10.795" y1="2.54" x2="-9.525" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-9.525" y1="2.54" x2="-8.89" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="1.905" x2="-8.255" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-8.255" y1="2.54" x2="-6.985" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-6.985" y1="2.54" x2="-6.35" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="1.905" x2="-5.715" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-5.715" y1="2.54" x2="-4.445" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="2.54" x2="-3.81" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="1.905" x2="-3.175" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="2.54" x2="-1.905" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="2.54" x2="-1.27" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="1.905" x2="-0.635" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="2.54" x2="0.635" y2="2.54" width="0.1524" layer="21"/>
<wire x1="0.635" y1="2.54" x2="1.27" y2="1.905" width="0.1524" layer="21"/>
<wire x1="1.27" y1="1.905" x2="1.905" y2="2.54" width="0.1524" layer="21"/>
<wire x1="1.905" y1="2.54" x2="3.175" y2="2.54" width="0.1524" layer="21"/>
<wire x1="3.175" y1="2.54" x2="3.81" y2="1.905" width="0.1524" layer="21"/>
<wire x1="3.81" y1="1.905" x2="4.445" y2="2.54" width="0.1524" layer="21"/>
<wire x1="4.445" y1="2.54" x2="5.715" y2="2.54" width="0.1524" layer="21"/>
<wire x1="5.715" y1="2.54" x2="6.35" y2="1.905" width="0.1524" layer="21"/>
<wire x1="6.35" y1="1.905" x2="6.985" y2="2.54" width="0.1524" layer="21"/>
<wire x1="6.985" y1="2.54" x2="8.255" y2="2.54" width="0.1524" layer="21"/>
<wire x1="8.255" y1="2.54" x2="8.89" y2="1.905" width="0.1524" layer="21"/>
<wire x1="8.89" y1="1.905" x2="9.525" y2="2.54" width="0.1524" layer="21"/>
<wire x1="9.525" y1="2.54" x2="10.795" y2="2.54" width="0.1524" layer="21"/>
<wire x1="10.795" y1="2.54" x2="11.43" y2="1.905" width="0.1524" layer="21"/>
<wire x1="11.43" y1="1.905" x2="12.065" y2="2.54" width="0.1524" layer="21"/>
<wire x1="12.065" y1="2.54" x2="13.335" y2="2.54" width="0.1524" layer="21"/>
<wire x1="13.97" y1="1.905" x2="13.335" y2="2.54" width="0.1524" layer="21"/>
<wire x1="13.97" y1="-1.905" x2="13.335" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="13.335" y1="-2.54" x2="12.065" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="11.43" y1="-1.905" x2="12.065" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="11.43" y1="-1.905" x2="10.795" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="10.795" y1="-2.54" x2="9.525" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="8.89" y1="-1.905" x2="9.525" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="8.89" y1="-1.905" x2="8.255" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="8.255" y1="-2.54" x2="6.985" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="6.35" y1="-1.905" x2="6.985" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="6.35" y1="-1.905" x2="5.715" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="5.715" y1="-2.54" x2="4.445" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-1.905" x2="4.445" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-1.905" x2="3.175" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-2.54" x2="1.905" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="1.905" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="0.635" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-2.54" x2="-0.635" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-1.905" x2="-0.635" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-13.97" y1="1.905" x2="-13.97" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-11.43" y1="1.905" x2="-11.43" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="1.905" x2="-8.89" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="1.905" x2="-6.35" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="1.905" x2="-3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="1.905" x2="-1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="1.27" y1="1.905" x2="1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="3.81" y1="1.905" x2="3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="6.35" y1="1.905" x2="6.35" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="8.89" y1="1.905" x2="8.89" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="11.43" y1="1.905" x2="11.43" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="13.97" y1="1.905" x2="13.97" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="13.97" y1="1.905" x2="14.605" y2="2.54" width="0.1524" layer="21"/>
<wire x1="14.605" y1="2.54" x2="15.875" y2="2.54" width="0.1524" layer="21"/>
<wire x1="16.51" y1="1.905" x2="15.875" y2="2.54" width="0.1524" layer="21"/>
<wire x1="16.51" y1="-1.905" x2="15.875" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="15.875" y1="-2.54" x2="14.605" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="13.97" y1="-1.905" x2="14.605" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="16.51" y1="1.905" x2="16.51" y2="-1.905" width="0.1524" layer="21"/>
<pad name="1" x="-15.24" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="2" x="-15.24" y="1.27" drill="1.016" shape="octagon"/>
<pad name="3" x="-12.7" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="4" x="-12.7" y="1.27" drill="1.016" shape="octagon"/>
<pad name="5" x="-10.16" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="6" x="-10.16" y="1.27" drill="1.016" shape="octagon"/>
<pad name="7" x="-7.62" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="8" x="-7.62" y="1.27" drill="1.016" shape="octagon"/>
<pad name="9" x="-5.08" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="10" x="-5.08" y="1.27" drill="1.016" shape="octagon"/>
<pad name="11" x="-2.54" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="12" x="-2.54" y="1.27" drill="1.016" shape="octagon"/>
<pad name="13" x="0" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="14" x="0" y="1.27" drill="1.016" shape="octagon"/>
<pad name="15" x="2.54" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="16" x="2.54" y="1.27" drill="1.016" shape="octagon"/>
<pad name="17" x="5.08" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="18" x="5.08" y="1.27" drill="1.016" shape="octagon"/>
<pad name="19" x="7.62" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="20" x="7.62" y="1.27" drill="1.016" shape="octagon"/>
<pad name="21" x="10.16" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="22" x="10.16" y="1.27" drill="1.016" shape="octagon"/>
<pad name="23" x="12.7" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="24" x="12.7" y="1.27" drill="1.016" shape="octagon"/>
<pad name="25" x="15.24" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="26" x="15.24" y="1.27" drill="1.016" shape="octagon"/>
<text x="-16.51" y="3.175" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-16.51" y="-4.445" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-15.494" y1="-1.524" x2="-14.986" y2="-1.016" layer="21"/>
<rectangle x1="-15.494" y1="1.016" x2="-14.986" y2="1.524" layer="21"/>
<rectangle x1="-12.954" y1="1.016" x2="-12.446" y2="1.524" layer="21"/>
<rectangle x1="-12.954" y1="-1.524" x2="-12.446" y2="-1.016" layer="21"/>
<rectangle x1="-10.414" y1="1.016" x2="-9.906" y2="1.524" layer="21"/>
<rectangle x1="-10.414" y1="-1.524" x2="-9.906" y2="-1.016" layer="21"/>
<rectangle x1="-7.874" y1="1.016" x2="-7.366" y2="1.524" layer="21"/>
<rectangle x1="-5.334" y1="1.016" x2="-4.826" y2="1.524" layer="21"/>
<rectangle x1="-2.794" y1="1.016" x2="-2.286" y2="1.524" layer="21"/>
<rectangle x1="-7.874" y1="-1.524" x2="-7.366" y2="-1.016" layer="21"/>
<rectangle x1="-5.334" y1="-1.524" x2="-4.826" y2="-1.016" layer="21"/>
<rectangle x1="-2.794" y1="-1.524" x2="-2.286" y2="-1.016" layer="21"/>
<rectangle x1="-0.254" y1="1.016" x2="0.254" y2="1.524" layer="21"/>
<rectangle x1="-0.254" y1="-1.524" x2="0.254" y2="-1.016" layer="21"/>
<rectangle x1="2.286" y1="1.016" x2="2.794" y2="1.524" layer="21"/>
<rectangle x1="2.286" y1="-1.524" x2="2.794" y2="-1.016" layer="21"/>
<rectangle x1="4.826" y1="1.016" x2="5.334" y2="1.524" layer="21"/>
<rectangle x1="4.826" y1="-1.524" x2="5.334" y2="-1.016" layer="21"/>
<rectangle x1="7.366" y1="1.016" x2="7.874" y2="1.524" layer="21"/>
<rectangle x1="7.366" y1="-1.524" x2="7.874" y2="-1.016" layer="21"/>
<rectangle x1="9.906" y1="1.016" x2="10.414" y2="1.524" layer="21"/>
<rectangle x1="9.906" y1="-1.524" x2="10.414" y2="-1.016" layer="21"/>
<rectangle x1="12.446" y1="1.016" x2="12.954" y2="1.524" layer="21"/>
<rectangle x1="12.446" y1="-1.524" x2="12.954" y2="-1.016" layer="21"/>
<rectangle x1="14.986" y1="1.016" x2="15.494" y2="1.524" layer="21"/>
<rectangle x1="14.986" y1="-1.524" x2="15.494" y2="-1.016" layer="21"/>
</package>
<package name="2X13/90" urn="urn:adsk.eagle:footprint:22281/1" library_version="5">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-16.51" y1="-1.905" x2="-13.97" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-13.97" y1="-1.905" x2="-13.97" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-13.97" y1="0.635" x2="-16.51" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-16.51" y1="0.635" x2="-16.51" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-15.24" y1="6.985" x2="-15.24" y2="1.27" width="0.762" layer="21"/>
<wire x1="-13.97" y1="-1.905" x2="-11.43" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-11.43" y1="-1.905" x2="-11.43" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-11.43" y1="0.635" x2="-13.97" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-12.7" y1="6.985" x2="-12.7" y2="1.27" width="0.762" layer="21"/>
<wire x1="-11.43" y1="-1.905" x2="-8.89" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="-1.905" x2="-8.89" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-8.89" y1="0.635" x2="-11.43" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-10.16" y1="6.985" x2="-10.16" y2="1.27" width="0.762" layer="21"/>
<wire x1="-8.89" y1="-1.905" x2="-6.35" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="-1.905" x2="-6.35" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="0.635" x2="-8.89" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-7.62" y1="6.985" x2="-7.62" y2="1.27" width="0.762" layer="21"/>
<wire x1="-6.35" y1="-1.905" x2="-3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="-1.905" x2="-3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="0.635" x2="-6.35" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="6.985" x2="-5.08" y2="1.27" width="0.762" layer="21"/>
<wire x1="-3.81" y1="-1.905" x2="-1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-1.905" x2="-1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="6.985" x2="-2.54" y2="1.27" width="0.762" layer="21"/>
<wire x1="-1.27" y1="-1.905" x2="1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="0.635" x2="-1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="0" y1="6.985" x2="0" y2="1.27" width="0.762" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-1.905" x2="3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="3.81" y1="0.635" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="2.54" y1="6.985" x2="2.54" y2="1.27" width="0.762" layer="21"/>
<wire x1="3.81" y1="-1.905" x2="6.35" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="6.35" y1="-1.905" x2="6.35" y2="0.635" width="0.1524" layer="21"/>
<wire x1="6.35" y1="0.635" x2="3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="5.08" y1="6.985" x2="5.08" y2="1.27" width="0.762" layer="21"/>
<wire x1="6.35" y1="-1.905" x2="8.89" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="8.89" y1="-1.905" x2="8.89" y2="0.635" width="0.1524" layer="21"/>
<wire x1="8.89" y1="0.635" x2="6.35" y2="0.635" width="0.1524" layer="21"/>
<wire x1="7.62" y1="6.985" x2="7.62" y2="1.27" width="0.762" layer="21"/>
<wire x1="8.89" y1="-1.905" x2="11.43" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="11.43" y1="-1.905" x2="11.43" y2="0.635" width="0.1524" layer="21"/>
<wire x1="11.43" y1="0.635" x2="8.89" y2="0.635" width="0.1524" layer="21"/>
<wire x1="10.16" y1="6.985" x2="10.16" y2="1.27" width="0.762" layer="21"/>
<wire x1="11.43" y1="-1.905" x2="13.97" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="13.97" y1="-1.905" x2="13.97" y2="0.635" width="0.1524" layer="21"/>
<wire x1="13.97" y1="0.635" x2="11.43" y2="0.635" width="0.1524" layer="21"/>
<wire x1="12.7" y1="6.985" x2="12.7" y2="1.27" width="0.762" layer="21"/>
<wire x1="13.97" y1="-1.905" x2="16.51" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="16.51" y1="-1.905" x2="16.51" y2="0.635" width="0.1524" layer="21"/>
<wire x1="16.51" y1="0.635" x2="13.97" y2="0.635" width="0.1524" layer="21"/>
<wire x1="15.24" y1="6.985" x2="15.24" y2="1.27" width="0.762" layer="21"/>
<pad name="2" x="-15.24" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="4" x="-12.7" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="6" x="-10.16" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="8" x="-7.62" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="10" x="-5.08" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="12" x="-2.54" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="14" x="0" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="16" x="2.54" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="18" x="5.08" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="20" x="7.62" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="22" x="10.16" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="24" x="12.7" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="26" x="15.24" y="-3.81" drill="1.016" shape="octagon"/>
<pad name="1" x="-15.24" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="3" x="-12.7" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="5" x="-10.16" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="7" x="-7.62" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="9" x="-5.08" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="11" x="-2.54" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="13" x="0" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="15" x="2.54" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="17" x="5.08" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="19" x="7.62" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="21" x="10.16" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="23" x="12.7" y="-6.35" drill="1.016" shape="octagon"/>
<pad name="25" x="15.24" y="-6.35" drill="1.016" shape="octagon"/>
<text x="-17.145" y="-3.81" size="1.27" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="18.415" y="-3.81" size="1.27" layer="27" rot="R90">&gt;VALUE</text>
<rectangle x1="-15.621" y1="0.635" x2="-14.859" y2="1.143" layer="21"/>
<rectangle x1="-13.081" y1="0.635" x2="-12.319" y2="1.143" layer="21"/>
<rectangle x1="-10.541" y1="0.635" x2="-9.779" y2="1.143" layer="21"/>
<rectangle x1="-8.001" y1="0.635" x2="-7.239" y2="1.143" layer="21"/>
<rectangle x1="-5.461" y1="0.635" x2="-4.699" y2="1.143" layer="21"/>
<rectangle x1="-2.921" y1="0.635" x2="-2.159" y2="1.143" layer="21"/>
<rectangle x1="-0.381" y1="0.635" x2="0.381" y2="1.143" layer="21"/>
<rectangle x1="2.159" y1="0.635" x2="2.921" y2="1.143" layer="21"/>
<rectangle x1="4.699" y1="0.635" x2="5.461" y2="1.143" layer="21"/>
<rectangle x1="7.239" y1="0.635" x2="8.001" y2="1.143" layer="21"/>
<rectangle x1="9.779" y1="0.635" x2="10.541" y2="1.143" layer="21"/>
<rectangle x1="12.319" y1="0.635" x2="13.081" y2="1.143" layer="21"/>
<rectangle x1="14.859" y1="0.635" x2="15.621" y2="1.143" layer="21"/>
<rectangle x1="-15.621" y1="-2.921" x2="-14.859" y2="-1.905" layer="21"/>
<rectangle x1="-13.081" y1="-2.921" x2="-12.319" y2="-1.905" layer="21"/>
<rectangle x1="-15.621" y1="-5.461" x2="-14.859" y2="-4.699" layer="21"/>
<rectangle x1="-15.621" y1="-4.699" x2="-14.859" y2="-2.921" layer="51"/>
<rectangle x1="-13.081" y1="-4.699" x2="-12.319" y2="-2.921" layer="51"/>
<rectangle x1="-13.081" y1="-5.461" x2="-12.319" y2="-4.699" layer="21"/>
<rectangle x1="-10.541" y1="-2.921" x2="-9.779" y2="-1.905" layer="21"/>
<rectangle x1="-8.001" y1="-2.921" x2="-7.239" y2="-1.905" layer="21"/>
<rectangle x1="-10.541" y1="-5.461" x2="-9.779" y2="-4.699" layer="21"/>
<rectangle x1="-10.541" y1="-4.699" x2="-9.779" y2="-2.921" layer="51"/>
<rectangle x1="-8.001" y1="-4.699" x2="-7.239" y2="-2.921" layer="51"/>
<rectangle x1="-8.001" y1="-5.461" x2="-7.239" y2="-4.699" layer="21"/>
<rectangle x1="-5.461" y1="-2.921" x2="-4.699" y2="-1.905" layer="21"/>
<rectangle x1="-2.921" y1="-2.921" x2="-2.159" y2="-1.905" layer="21"/>
<rectangle x1="-5.461" y1="-5.461" x2="-4.699" y2="-4.699" layer="21"/>
<rectangle x1="-5.461" y1="-4.699" x2="-4.699" y2="-2.921" layer="51"/>
<rectangle x1="-2.921" y1="-4.699" x2="-2.159" y2="-2.921" layer="51"/>
<rectangle x1="-2.921" y1="-5.461" x2="-2.159" y2="-4.699" layer="21"/>
<rectangle x1="-0.381" y1="-2.921" x2="0.381" y2="-1.905" layer="21"/>
<rectangle x1="2.159" y1="-2.921" x2="2.921" y2="-1.905" layer="21"/>
<rectangle x1="-0.381" y1="-5.461" x2="0.381" y2="-4.699" layer="21"/>
<rectangle x1="-0.381" y1="-4.699" x2="0.381" y2="-2.921" layer="51"/>
<rectangle x1="2.159" y1="-4.699" x2="2.921" y2="-2.921" layer="51"/>
<rectangle x1="2.159" y1="-5.461" x2="2.921" y2="-4.699" layer="21"/>
<rectangle x1="4.699" y1="-2.921" x2="5.461" y2="-1.905" layer="21"/>
<rectangle x1="7.239" y1="-2.921" x2="8.001" y2="-1.905" layer="21"/>
<rectangle x1="4.699" y1="-5.461" x2="5.461" y2="-4.699" layer="21"/>
<rectangle x1="4.699" y1="-4.699" x2="5.461" y2="-2.921" layer="51"/>
<rectangle x1="7.239" y1="-4.699" x2="8.001" y2="-2.921" layer="51"/>
<rectangle x1="7.239" y1="-5.461" x2="8.001" y2="-4.699" layer="21"/>
<rectangle x1="9.779" y1="-2.921" x2="10.541" y2="-1.905" layer="21"/>
<rectangle x1="12.319" y1="-2.921" x2="13.081" y2="-1.905" layer="21"/>
<rectangle x1="9.779" y1="-5.461" x2="10.541" y2="-4.699" layer="21"/>
<rectangle x1="9.779" y1="-4.699" x2="10.541" y2="-2.921" layer="51"/>
<rectangle x1="12.319" y1="-4.699" x2="13.081" y2="-2.921" layer="51"/>
<rectangle x1="12.319" y1="-5.461" x2="13.081" y2="-4.699" layer="21"/>
<rectangle x1="14.859" y1="-2.921" x2="15.621" y2="-1.905" layer="21"/>
<rectangle x1="14.859" y1="-5.461" x2="15.621" y2="-4.699" layer="21"/>
<rectangle x1="14.859" y1="-4.699" x2="15.621" y2="-2.921" layer="51"/>
</package>
</packages>
<packages3d>
<package3d name="2X13" urn="urn:adsk.eagle:package:22418/2" type="model" library_version="5">
<description>PIN HEADER</description>
<packageinstances>
<packageinstance name="2X13"/>
</packageinstances>
</package3d>
<package3d name="2X13/90" urn="urn:adsk.eagle:package:22425/2" type="model" library_version="5">
<description>PIN HEADER</description>
<packageinstances>
<packageinstance name="2X13/90"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="PINH2X13" urn="urn:adsk.eagle:symbol:22282/1" library_version="5">
<wire x1="-6.35" y1="-17.78" x2="8.89" y2="-17.78" width="0.4064" layer="94"/>
<wire x1="8.89" y1="-17.78" x2="8.89" y2="17.78" width="0.4064" layer="94"/>
<wire x1="8.89" y1="17.78" x2="-6.35" y2="17.78" width="0.4064" layer="94"/>
<wire x1="-6.35" y1="17.78" x2="-6.35" y2="-17.78" width="0.4064" layer="94"/>
<text x="-6.35" y="18.415" size="1.778" layer="95">&gt;NAME</text>
<text x="-6.35" y="-20.32" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-2.54" y="15.24" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="2" x="5.08" y="15.24" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="3" x="-2.54" y="12.7" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="4" x="5.08" y="12.7" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="5" x="-2.54" y="10.16" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="6" x="5.08" y="10.16" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="7" x="-2.54" y="7.62" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="8" x="5.08" y="7.62" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="9" x="-2.54" y="5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="10" x="5.08" y="5.08" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="11" x="-2.54" y="2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="12" x="5.08" y="2.54" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="13" x="-2.54" y="0" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="14" x="5.08" y="0" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="15" x="-2.54" y="-2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="16" x="5.08" y="-2.54" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="17" x="-2.54" y="-5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="18" x="5.08" y="-5.08" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="19" x="-2.54" y="-7.62" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="20" x="5.08" y="-7.62" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="21" x="-2.54" y="-10.16" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="22" x="5.08" y="-10.16" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="23" x="-2.54" y="-12.7" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="24" x="5.08" y="-12.7" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="25" x="-2.54" y="-15.24" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="26" x="5.08" y="-15.24" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="PINHD-2X13" urn="urn:adsk.eagle:component:22510/4" prefix="JP" uservalue="yes" library_version="5">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="A" symbol="PINH2X13" x="0" y="0"/>
</gates>
<devices>
<device name="" package="2X13">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="10" pad="10"/>
<connect gate="A" pin="11" pad="11"/>
<connect gate="A" pin="12" pad="12"/>
<connect gate="A" pin="13" pad="13"/>
<connect gate="A" pin="14" pad="14"/>
<connect gate="A" pin="15" pad="15"/>
<connect gate="A" pin="16" pad="16"/>
<connect gate="A" pin="17" pad="17"/>
<connect gate="A" pin="18" pad="18"/>
<connect gate="A" pin="19" pad="19"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="20" pad="20"/>
<connect gate="A" pin="21" pad="21"/>
<connect gate="A" pin="22" pad="22"/>
<connect gate="A" pin="23" pad="23"/>
<connect gate="A" pin="24" pad="24"/>
<connect gate="A" pin="25" pad="25"/>
<connect gate="A" pin="26" pad="26"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
<connect gate="A" pin="9" pad="9"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:22418/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="POPULARITY" value="4" constant="no"/>
</technology>
</technologies>
</device>
<device name="/90" package="2X13/90">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="10" pad="10"/>
<connect gate="A" pin="11" pad="11"/>
<connect gate="A" pin="12" pad="12"/>
<connect gate="A" pin="13" pad="13"/>
<connect gate="A" pin="14" pad="14"/>
<connect gate="A" pin="15" pad="15"/>
<connect gate="A" pin="16" pad="16"/>
<connect gate="A" pin="17" pad="17"/>
<connect gate="A" pin="18" pad="18"/>
<connect gate="A" pin="19" pad="19"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="20" pad="20"/>
<connect gate="A" pin="21" pad="21"/>
<connect gate="A" pin="22" pad="22"/>
<connect gate="A" pin="23" pad="23"/>
<connect gate="A" pin="24" pad="24"/>
<connect gate="A" pin="25" pad="25"/>
<connect gate="A" pin="26" pad="26"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
<connect gate="A" pin="9" pad="9"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:22425/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="POPULARITY" value="0" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="U1" library="raspberry_pi_3b" deviceset="RASPBERRY_PI_3B+_HDR" device="MODEL_RASPBEERY_PI_3B_HDR"/>
<part name="JP1" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-2X13" device="" package3d_urn="urn:adsk.eagle:package:22418/2"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="U1" gate="G$1" x="76.2" y="50.8" smashed="yes">
<attribute name="NAME" x="55.88" y="84.328" size="2.54" layer="95"/>
<attribute name="VALUE" x="55.88" y="17.78" size="2.54" layer="96"/>
</instance>
<instance part="JP1" gate="A" x="5.08" y="50.8" smashed="yes">
<attribute name="NAME" x="-1.27" y="69.215" size="1.778" layer="95"/>
<attribute name="VALUE" x="-1.27" y="30.48" size="1.778" layer="96"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="+5V" class="0">
<segment>
<pinref part="JP1" gate="A" pin="2"/>
<wire x1="10.16" y1="66.04" x2="17.78" y2="66.04" width="0.1524" layer="91"/>
<wire x1="17.78" y1="66.04" x2="17.78" y2="63.5" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="4"/>
<wire x1="17.78" y1="63.5" x2="10.16" y2="63.5" width="0.1524" layer="91"/>
<wire x1="17.78" y1="66.04" x2="17.78" y2="88.9" width="0.1524" layer="91"/>
<junction x="17.78" y="66.04"/>
<wire x1="17.78" y1="88.9" x2="106.68" y2="88.9" width="0.1524" layer="91"/>
<wire x1="106.68" y1="88.9" x2="106.68" y2="81.28" width="0.1524" layer="91"/>
<pinref part="U1" gate="G$1" pin="5V"/>
<wire x1="106.68" y1="81.28" x2="101.6" y2="81.28" width="0.1524" layer="91"/>
<label x="58.42" y="88.9" size="1.778" layer="95"/>
</segment>
</net>
<net name="+3.3V" class="0">
<segment>
<pinref part="U1" gate="G$1" pin="3V3"/>
<wire x1="101.6" y1="78.74" x2="109.22" y2="78.74" width="0.1524" layer="91"/>
<wire x1="109.22" y1="78.74" x2="109.22" y2="91.44" width="0.1524" layer="91"/>
<wire x1="109.22" y1="91.44" x2="-10.16" y2="91.44" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="91.44" x2="-10.16" y2="66.04" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="1"/>
<wire x1="-10.16" y1="66.04" x2="-5.08" y2="66.04" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="17"/>
<wire x1="-5.08" y1="66.04" x2="2.54" y2="66.04" width="0.1524" layer="91"/>
<wire x1="2.54" y1="45.72" x2="-5.08" y2="45.72" width="0.1524" layer="91"/>
<wire x1="-5.08" y1="45.72" x2="-5.08" y2="66.04" width="0.1524" layer="91"/>
<junction x="-5.08" y="66.04"/>
<label x="60.96" y="91.44" size="1.778" layer="95"/>
</segment>
</net>
<net name="CE1" class="0">
<segment>
<pinref part="JP1" gate="A" pin="26"/>
<wire x1="10.16" y1="35.56" x2="30.48" y2="35.56" width="0.1524" layer="91"/>
<pinref part="U1" gate="G$1" pin="GPIO7/!SPI_CE1!"/>
<wire x1="30.48" y1="35.56" x2="30.48" y2="43.18" width="0.1524" layer="91"/>
<wire x1="30.48" y1="43.18" x2="50.8" y2="43.18" width="0.1524" layer="91"/>
<label x="33.02" y="43.18" size="1.778" layer="95"/>
</segment>
</net>
<net name="CE0" class="0">
<segment>
<pinref part="U1" gate="G$1" pin="GPIO8/!SPI_CE0!"/>
<wire x1="50.8" y1="45.72" x2="27.94" y2="45.72" width="0.1524" layer="91"/>
<wire x1="27.94" y1="45.72" x2="27.94" y2="38.1" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="24"/>
<wire x1="27.94" y1="38.1" x2="10.16" y2="38.1" width="0.1524" layer="91"/>
<label x="33.02" y="45.72" size="1.778" layer="95"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="JP1" gate="A" pin="9"/>
<wire x1="2.54" y1="55.88" x2="-10.16" y2="55.88" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="55.88" x2="-10.16" y2="35.56" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="25"/>
<wire x1="-10.16" y1="35.56" x2="2.54" y2="35.56" width="0.1524" layer="91"/>
<pinref part="U1" gate="G$1" pin="GND"/>
<wire x1="101.6" y1="22.86" x2="106.68" y2="22.86" width="0.1524" layer="91"/>
<wire x1="106.68" y1="22.86" x2="106.68" y2="12.7" width="0.1524" layer="91"/>
<wire x1="106.68" y1="12.7" x2="17.78" y2="12.7" width="0.1524" layer="91"/>
<wire x1="17.78" y1="12.7" x2="17.78" y2="30.48" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="6"/>
<wire x1="17.78" y1="30.48" x2="17.78" y2="43.18" width="0.1524" layer="91"/>
<wire x1="17.78" y1="43.18" x2="17.78" y2="60.96" width="0.1524" layer="91"/>
<wire x1="17.78" y1="60.96" x2="10.16" y2="60.96" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="35.56" x2="-10.16" y2="30.48" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="30.48" x2="17.78" y2="30.48" width="0.1524" layer="91"/>
<junction x="-10.16" y="35.56"/>
<junction x="17.78" y="30.48"/>
<label x="30.48" y="10.16" size="1.778" layer="95"/>
<pinref part="JP1" gate="A" pin="20"/>
<wire x1="10.16" y1="43.18" x2="17.78" y2="43.18" width="0.1524" layer="91"/>
<junction x="17.78" y="43.18"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="U1" gate="G$1" pin="GPIO11/SPI_SCLK"/>
<wire x1="50.8" y1="40.64" x2="33.02" y2="40.64" width="0.1524" layer="91"/>
<wire x1="33.02" y1="40.64" x2="33.02" y2="25.4" width="0.1524" layer="91"/>
<wire x1="33.02" y1="25.4" x2="-12.7" y2="25.4" width="0.1524" layer="91"/>
<wire x1="-12.7" y1="25.4" x2="-12.7" y2="38.1" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="23"/>
<wire x1="-12.7" y1="38.1" x2="2.54" y2="38.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="U1" gate="G$1" pin="GPIO10/SPI_MOSI"/>
<wire x1="50.8" y1="38.1" x2="35.56" y2="38.1" width="0.1524" layer="91"/>
<wire x1="35.56" y1="38.1" x2="35.56" y2="22.86" width="0.1524" layer="91"/>
<wire x1="35.56" y1="22.86" x2="-15.24" y2="22.86" width="0.1524" layer="91"/>
<wire x1="-15.24" y1="22.86" x2="-15.24" y2="43.18" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="19"/>
<wire x1="-15.24" y1="43.18" x2="2.54" y2="43.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="U1" gate="G$1" pin="GPIO9/SPI_MISO"/>
<wire x1="50.8" y1="35.56" x2="38.1" y2="35.56" width="0.1524" layer="91"/>
<wire x1="38.1" y1="35.56" x2="38.1" y2="20.32" width="0.1524" layer="91"/>
<wire x1="38.1" y1="20.32" x2="-17.78" y2="20.32" width="0.1524" layer="91"/>
<wire x1="-17.78" y1="20.32" x2="-17.78" y2="40.64" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="21"/>
<wire x1="-17.78" y1="40.64" x2="2.54" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="U1" gate="G$1" pin="GPIO25/GPIO_GEN6"/>
<wire x1="50.8" y1="50.8" x2="25.4" y2="50.8" width="0.1524" layer="91"/>
<wire x1="25.4" y1="50.8" x2="25.4" y2="40.64" width="0.1524" layer="91"/>
<pinref part="JP1" gate="A" pin="22"/>
<wire x1="25.4" y1="40.64" x2="10.16" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports the association of 3D packages
with devices in libraries, schematics, and board files. Those 3D
packages will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>
