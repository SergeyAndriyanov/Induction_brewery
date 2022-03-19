# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS 

"audioalarmsensor.c"
"audiobboilndone.c"
"audioboilnrun.c"
"audioconnect.c"
"audiodisconnect.c"
"audiohopsadd.c"
"audiokeypress.c"
"audioloadsystem.c"
"audiomeshdone.c"
"audiomeshrun.c"
"audiotermopause.c"
"CRCTable.c"

"dac_aud.c"
"datablock.c"
"hw_init.c"
"inductionboard.c"
"key.c"
"lcd.c"
"main_cod.c"
"MAX31865.c"
"menu.c"
"modbus.c"
"process_work.c"
"wifi.c"

)
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
