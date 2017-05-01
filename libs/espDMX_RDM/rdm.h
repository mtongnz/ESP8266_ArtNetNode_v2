/*****************************************************************/
/* Entertainment Services Technology Association (ESTA)          */
/* ANSI E1.20 Remote Device Management (RDM) over DMX512 Networks*/
/*****************************************************************/
/*                                                               */
/*                          RDM.h                                */
/*                                                               */
/*****************************************************************/
/* Appendix A Defines for the RDM Protocol.                      */
/* Publish date: 3/31/2006                                       */
/*****************************************************************/
/* Compiled by: Scott M. Blair   8/18/2006                       */
/* Updated 10/11/2011: Adding E1.20-2010 and E1.37-1 defines.    */
/*****************************************************************/
/* For updates see: http://www.rdmprotocol.org                   */
/*****************************************************************/
/* Copyright 2006,2011 Litespeed Design                          */
/*****************************************************************/
/* Permission to use, copy, modify, and distribute this software */
/* is freely granted, provided that this notice is preserved.    */
/*****************************************************************/

/* Protocol version. */
#define E120_PROTOCOL_VERSION                             0x0100

/* RDM START CODE (Slot 0)                                                                                                     */
#define  E120_SC_RDM                        0xCC

/* RDM Protocol Data Structure ID's (Slot 1)                                                                                   */
#define E120_SC_SUB_MESSAGE                 0x01

/* Broadcast Device UID's                                                                                                      */
#define E120_BROADCAST_ALL_DEVICES_ID               0xFFFFFFFFFFFF   /* (Broadcast all Manufacturers)                    */
//#define ALL_DEVICES_ID                      0xmmmmFFFFFFFF   /* (Specific Manufacturer ID 0xmmmm)                                              */

#define E120_SUB_DEVICE_ALL_CALL                0xFFFF


/********************************************************/
/* Table A-1: RDM Command Classes (Slot 20)             */
/********************************************************/
#define E120_DISCOVERY_COMMAND                            0x10
#define E120_DISCOVERY_COMMAND_RESPONSE                   0x11
#define E120_GET_COMMAND                                  0x20
#define E120_GET_COMMAND_RESPONSE                         0x21
#define E120_SET_COMMAND                                  0x30
#define E120_SET_COMMAND_RESPONSE                         0x31



/********************************************************/
/* Table A-2: RDM Response Type (Slot 16)               */
/********************************************************/
#define E120_RESPONSE_TYPE_ACK                            0x00
#define E120_RESPONSE_TYPE_ACK_TIMER                      0x01
#define E120_RESPONSE_TYPE_NACK_REASON                    0x02   /* See Table A-17                                              */
#define E120_RESPONSE_TYPE_ACK_OVERFLOW                   0x03   /* Additional Response Data available beyond single response length.*/


/********************************************************/
/* Table A-3: RDM Parameter ID's (Slots 21-22)          */
/********************************************************/
/* Category - Network Management   */
#define E120_DISC_UNIQUE_BRANCH                           0x0001
#define E120_DISC_MUTE                                    0x0002
#define E120_DISC_UN_MUTE                                 0x0003
#define E120_PROXIED_DEVICES                              0x0010
#define E120_PROXIED_DEVICE_COUNT                         0x0011
#define E120_COMMS_STATUS                                 0x0015

/* Category - Status Collection    */
#define E120_QUEUED_MESSAGE                               0x0020 /* See Table A-4                                              */
#define E120_STATUS_MESSAGES                              0x0030 /* See Table A-4                                              */
#define E120_STATUS_ID_DESCRIPTION                        0x0031
#define E120_CLEAR_STATUS_ID                              0x0032
#define E120_SUB_DEVICE_STATUS_REPORT_THRESHOLD           0x0033 /* See Table A-4                                              */

/* Category - RDM Information      */
#define E120_SUPPORTED_PARAMETERS                         0x0050 /* Support required only if supporting Parameters beyond the minimum required set.*/
#define E120_PARAMETER_DESCRIPTION                        0x0051 /* Support required for Manufacturer-Specific PIDs exposed in SUPPORTED_PARAMETERS message */

/* Category - Product Information  */
#define E120_DEVICE_INFO                                  0x0060
#define E120_PRODUCT_DETAIL_ID_LIST                       0x0070
#define E120_DEVICE_MODEL_DESCRIPTION                     0x0080
#define E120_MANUFACTURER_LABEL                           0x0081
#define E120_DEVICE_LABEL                                 0x0082
#define E120_FACTORY_DEFAULTS                             0x0090
#define E120_LANGUAGE_CAPABILITIES                        0x00A0
#define E120_LANGUAGE                                     0x00B0
#define E120_SOFTWARE_VERSION_LABEL                       0x00C0
#define E120_BOOT_SOFTWARE_VERSION_ID                     0x00C1
#define E120_BOOT_SOFTWARE_VERSION_LABEL                  0x00C2

/* Category - DMX512 Setup         */
#define E120_DMX_PERSONALITY                              0x00E0
#define E120_DMX_PERSONALITY_DESCRIPTION                  0x00E1
#define E120_DMX_START_ADDRESS                            0x00F0 /* Support required if device uses a DMX512 Slot.             */
#define E120_SLOT_INFO                                    0x0120
#define E120_SLOT_DESCRIPTION                             0x0121
#define E120_DEFAULT_SLOT_VALUE                           0x0122
#define E137_1_DMX_BLOCK_ADDRESS                          0x0140 /* Defined in ANSI E1.37-1 document                           */
#define E137_1_DMX_FAIL_MODE                              0x0141 /* Defined in ANSI E1.37-1 document                           */
#define E137_1_DMX_STARTUP_MODE                           0x0142 /* Defined in ANSI E1.37-1 document                           */


/* Category - Sensors              */
#define E120_SENSOR_DEFINITION                            0x0200
#define E120_SENSOR_VALUE                                 0x0201
#define E120_RECORD_SENSORS                               0x0202

/* Category - Dimmer Settings      */
#define E137_1_DIMMER_INFO                                0x0340
#define E137_1_MINIMUM_LEVEL                              0x0341
#define E137_1_MAXIMUM_LEVEL                              0x0342
#define E137_1_CURVE                                      0x0343
#define E137_1_CURVE_DESCRIPTION                          0x0344 /* Support required if CURVE is supported                     */
#define E137_1_OUTPUT_RESPONSE_TIME                       0x0345
#define E137_1_OUTPUT_RESPONSE_TIME_DESCRIPTION           0x0346 /* Support required if OUTPUT_RESPONSE_TIME is supported      */
#define E137_1_MODULATION_FREQUENCY                       0x0347
#define E137_1_MODULATION_FREQUENCY_DESCRIPTION           0x0348 /* Support required if MODULATION_FREQUENCY is supported      */

/* Category - Power/Lamp Settings  */
#define E120_DEVICE_HOURS                                 0x0400
#define E120_LAMP_HOURS                                   0x0401
#define E120_LAMP_STRIKES                                 0x0402
#define E120_LAMP_STATE                                   0x0403 /* See Table A-8                                              */
#define E120_LAMP_ON_MODE                                 0x0404 /* See Table A-9                                              */
#define E120_DEVICE_POWER_CYCLES                          0x0405
#define E137_1_BURN_IN                    0x0440 /* Defined in ANSI E1.37-1                                    */

/* Category - Display Settings     */
#define E120_DISPLAY_INVERT                               0x0500
#define E120_DISPLAY_LEVEL                                0x0501

/* Category - Configuration        */
#define E120_PAN_INVERT                                   0x0600
#define E120_TILT_INVERT                                  0x0601
#define E120_PAN_TILT_SWAP                                0x0602
#define E120_REAL_TIME_CLOCK                              0x0603
#define E137_1_LOCK_PIN                                   0x0640 /* Defined in ANSI E1.37-1                                    */
#define E137_1_LOCK_STATE                                 0x0641 /* Defined in ANSI E1.37-1                                    */
#define E137_1_LOCK_STATE_DESCRIPTION                     0x0642 /* Support required if MODULATION_FREQUENCY is supported      */

/* Category - Control              */
#define E120_IDENTIFY_DEVICE                              0x1000
#define E120_RESET_DEVICE                                 0x1001
#define E120_POWER_STATE                                  0x1010 /* See Table A-11                                              */
#define E120_PERFORM_SELFTEST                             0x1020 /* See Table A-10                                              */
#define E120_SELF_TEST_DESCRIPTION                        0x1021
#define E120_CAPTURE_PRESET                               0x1030
#define E120_PRESET_PLAYBACK                              0x1031 /* See Table A-7                                               */
#define E137_1_IDENTIFY_MODE                              0x1040 /* Defined in ANSI E1.37-1                                     */
#define E137_1_PRESET_INFO                                0x1041 /* Defined in ANSI E1.37-1                                     */
#define E137_1_PRESET_STATUS                              0x1042 /* Defined in ANSI E1.37-1                                     */
#define E137_1_PRESET_MERGEMODE                           0x1043 /* See E1.37-1 Table A-3                                       */
#define E137_1_POWER_ON_SELF_TEST                         0x1044 /* Defined in ANSI E1.37-1                                     */

/* ESTA Reserved Future RDM Development                   0x7FE0-
                                                          0x7FFF

   Manufacturer-Specific PIDs                             0x8000-
                                                          0xFFDF
   ESTA Reserved Future RDM Development
                                                          0xFFE0-
                                                          0xFFFF
*/


/*****************************************************************/
/* Discovery Mute/Un-Mute Messages Control Field. See Table 7-3. */
/*****************************************************************/
#define E120_CONTROL_PROXIED_DEVICE                       0x0008
#define E120_CONTROL_BOOT_LOADER                          0x0004
#define E120_CONTROL_SUB_DEVICE                           0x0002
#define E120_CONTROL_MANAGED_PROXY                        0x0001


/********************************************************/
/* Table A-4: Status Type Defines                       */
/********************************************************/
#define E120_STATUS_NONE                                  0x00   /* Not allowed for use with GET: QUEUED_MESSAGE                */
#define E120_STATUS_GET_LAST_MESSAGE                      0x01
#define E120_STATUS_ADVISORY                              0x02
#define E120_STATUS_WARNING                               0x03
#define E120_STATUS_ERROR                                 0x04
#define E120_STATUS_ADVISORY_CLEARED                      0x12  /* Added in E1.20-2010 version                                  */
#define E120_STATUS_WARNING_CLEARED                       0x13  /* Added in E1.20-2010 version                                  */
#define E120_STATUS_ERROR_CLEARED                         0x14  /* Added in E1.20-2010 version                                  */



/********************************************************/
/* Table A-5: Product Category Defines                  */
/********************************************************/
#define E120_PRODUCT_CATEGORY_NOT_DECLARED                0x0000

/* Fixtures - intended as source of illumination See Note 1                                                                     */
#define E120_PRODUCT_CATEGORY_FIXTURE                     0x0100 /* No Fine Category declared                                   */
#define E120_PRODUCT_CATEGORY_FIXTURE_FIXED               0x0101 /* No pan / tilt / focus style functions                       */
#define E120_PRODUCT_CATEGORY_FIXTURE_MOVING_YOKE         0x0102
#define E120_PRODUCT_CATEGORY_FIXTURE_MOVING_MIRROR       0x0103
#define E120_PRODUCT_CATEGORY_FIXTURE_OTHER               0x01FF /* For example, focus but no pan/tilt.                         */

/* Fixture Accessories - add-ons to fixtures or projectors                                                                      */
#define E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY           0x0200 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_COLOR     0x0201 /* Scrollers / Color Changers                                  */
#define E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_YOKE      0x0202 /* Yoke add-on                                                 */
#define E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_MIRROR    0x0203 /* Moving mirror add-on                                        */
#define E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_EFFECT    0x0204 /* Effects Discs                                               */
#define E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_BEAM      0x0205 /* Gobo Rotators /Iris / Shutters / Dousers/ Beam modifiers.   */
#define E120_PRODUCT_CATEGORY_FIXTURE_ACCESSORY_OTHER     0x02FF

/* Projectors - light source capable of producing realistic images from another media i.e Video / Slide / Oil Wheel / Film */
#define E120_PRODUCT_CATEGORY_PROJECTOR                   0x0300 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_PROJECTOR_FIXED             0x0301 /* No pan / tilt functions.                                    */
#define E120_PRODUCT_CATEGORY_PROJECTOR_MOVING_YOKE       0x0302
#define E120_PRODUCT_CATEGORY_PROJECTOR_MOVING_MIRROR     0x0303
#define E120_PRODUCT_CATEGORY_PROJECTOR_OTHER             0x03FF

/* Atmospheric Effect - earth/wind/fire                                                                                         */
#define E120_PRODUCT_CATEGORY_ATMOSPHERIC                 0x0400 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_ATMOSPHERIC_EFFECT          0x0401 /* Fogger / Hazer / Flame, etc.                                */
#define E120_PRODUCT_CATEGORY_ATMOSPHERIC_PYRO            0x0402 /* See Note 2.                                                 */
#define E120_PRODUCT_CATEGORY_ATMOSPHERIC_OTHER           0x04FF

/* Intensity Control (specifically Dimming equipment)                                                                           */
#define E120_PRODUCT_CATEGORY_DIMMER                      0x0500 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_DIMMER_AC_INCANDESCENT      0x0501 /* AC > 50VAC                                                  */
#define E120_PRODUCT_CATEGORY_DIMMER_AC_FLUORESCENT       0x0502
#define E120_PRODUCT_CATEGORY_DIMMER_AC_COLDCATHODE       0x0503 /* High Voltage outputs such as Neon or other cold cathode.    */
#define E120_PRODUCT_CATEGORY_DIMMER_AC_NONDIM            0x0504 /* Non-Dim module in dimmer rack.                              */
#define E120_PRODUCT_CATEGORY_DIMMER_AC_ELV               0x0505 /* AC <= 50V such as 12/24V AC Low voltage lamps.              */
#define E120_PRODUCT_CATEGORY_DIMMER_AC_OTHER             0x0506
#define E120_PRODUCT_CATEGORY_DIMMER_DC_LEVEL             0x0507 /* Variable DC level output.                                   */
#define E120_PRODUCT_CATEGORY_DIMMER_DC_PWM               0x0508 /* Chopped (PWM) output.                                       */
#define E120_PRODUCT_CATEGORY_DIMMER_CS_LED               0x0509 /* Specialized LED dimmer.                                     */
#define E120_PRODUCT_CATEGORY_DIMMER_OTHER                0x05FF

/* Power Control (other than Dimming equipment)                                                                                 */
#define E120_PRODUCT_CATEGORY_POWER                       0x0600 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_POWER_CONTROL               0x0601 /* Contactor racks, other forms of Power Controllers.          */
#define E120_PRODUCT_CATEGORY_POWER_SOURCE                0x0602 /* Generators                                                  */
#define E120_PRODUCT_CATEGORY_POWER_OTHER                 0x06FF

/* Scenic Drive - including motorized effects unrelated to light source.                                                        */
#define E120_PRODUCT_CATEGORY_SCENIC                      0x0700 /* No Fine Category declared                                   */
#define E120_PRODUCT_CATEGORY_SCENIC_DRIVE                0x0701 /* Rotators / Kabuki drops, etc. See Note 2.                   */
#define E120_PRODUCT_CATEGORY_SCENIC_OTHER                0x07FF

/* DMX Infrastructure, conversion and interfaces                                                                                */
#define E120_PRODUCT_CATEGORY_DATA                        0x0800 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_DATA_DISTRIBUTION           0x0801 /* Splitters/repeaters/Ethernet products used to distribute DMX*/
#define E120_PRODUCT_CATEGORY_DATA_CONVERSION             0x0802 /* Protocol Conversion analog decoders.                        */
#define E120_PRODUCT_CATEGORY_DATA_OTHER                  0x08FF

/* Audio-Visual Equipment                                                                                                       */
#define E120_PRODUCT_CATEGORY_AV                          0x0900 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_AV_AUDIO                    0x0901 /* Audio controller or device.                                 */
#define E120_PRODUCT_CATEGORY_AV_VIDEO                    0x0902 /* Video controller or display device.                         */
#define E120_PRODUCT_CATEGORY_AV_OTHER                    0x09FF

/* Parameter Monitoring Equipment See Note 3.                                                                                   */
#define E120_PRODUCT_CATEGORY_MONITOR                     0x0A00 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_MONITOR_ACLINEPOWER         0x0A01 /* Product that monitors AC line voltage, current or power.    */
#define E120_PRODUCT_CATEGORY_MONITOR_DCPOWER             0x0A02 /* Product that monitors DC line voltage, current or power.    */
#define E120_PRODUCT_CATEGORY_MONITOR_ENVIRONMENTAL       0x0A03 /* Temperature or other environmental parameter.               */
#define E120_PRODUCT_CATEGORY_MONITOR_OTHER               0x0AFF

/* Controllers, Backup devices                                                                                                  */
#define E120_PRODUCT_CATEGORY_CONTROL                     0x7000 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_CONTROL_CONTROLLER          0x7001
#define E120_PRODUCT_CATEGORY_CONTROL_BACKUPDEVICE        0x7002
#define E120_PRODUCT_CATEGORY_CONTROL_OTHER               0x70FF

/* Test Equipment                                                                                                               */
#define E120_PRODUCT_CATEGORY_TEST                        0x7100 /* No Fine Category declared.                                  */
#define E120_PRODUCT_CATEGORY_TEST_EQUIPMENT              0x7101
#define E120_PRODUCT_CATEGORY_TEST_EQUIPMENT_OTHER        0x71FF

/* Miscellaneous                                                                                                                */
#define E120_PRODUCT_CATEGORY_OTHER                       0x7FFF /* For devices that aren't described within this table.        */

/* Manufacturer Specific Categories                       0x8000 -
                                                          0xDFFF                                                                */


/********************************************************/
/* Table A-6: Product Detail Defines                    */
/********************************************************/

#define E120_PRODUCT_DETAIL_NOT DECLARED                  0x0000

/* Generally applied to fixtures                                                                                                */
#define E120_PRODUCT_DETAIL_ARC                           0x0001
#define E120_PRODUCT_DETAIL_METAL_HALIDE                  0x0002
#define E120_PRODUCT_DETAIL_INCANDESCENT                  0x0003
#define E120_PRODUCT_DETAIL_LED                           0x0004
#define E120_PRODUCT_DETAIL_FLUROESCENT                   0x0005
#define E120_PRODUCT_DETAIL_COLDCATHODE                   0x0006  /*includes Neon/Argon                                         */
#define E120_PRODUCT_DETAIL_ELECTROLUMINESCENT            0x0007
#define E120_PRODUCT_DETAIL_LASER                         0x0008
#define E120_PRODUCT_DETAIL_FLASHTUBE                     0x0009 /* Strobes or other flashtubes                                 */

/* Generally applied to fixture accessories                                                                                     */
#define E120_PRODUCT_DETAIL_COLORSCROLLER                 0x0100
#define E120_PRODUCT_DETAIL_COLORWHEEL                    0x0101
#define E120_PRODUCT_DETAIL_COLORCHANGE                   0x0102 /* Semaphore or other type                                     */
#define E120_PRODUCT_DETAIL_IRIS_DOUSER                   0x0103
#define E120_PRODUCT_DETAIL_DIMMING_SHUTTER               0x0104
#define E120_PRODUCT_DETAIL_PROFILE_SHUTTER               0x0105 /* hard-edge beam masking                                      */
#define E120_PRODUCT_DETAIL_BARNDOOR_SHUTTER              0x0106 /* soft-edge beam masking                                      */
#define E120_PRODUCT_DETAIL_EFFECTS_DISC                  0x0107
#define E120_PRODUCT_DETAIL_GOBO_ROTATOR                  0x0108

/* Generally applied to Projectors                                                                                              */
#define E120_PRODUCT_DETAIL_VIDEO                         0x0200
#define E120_PRODUCT_DETAIL_SLIDE                         0x0201
#define E120_PRODUCT_DETAIL_FILM                          0x0202
#define E120_PRODUCT_DETAIL_OILWHEEL                      0x0203
#define E120_PRODUCT_DETAIL_LCDGATE                       0x0204

/* Generally applied to Atmospheric Effects                                                                                     */
#define E120_PRODUCT_DETAIL_FOGGER_GLYCOL                 0x0300 /* Glycol/Glycerin hazer                                       */
#define E120_PRODUCT_DETAIL_FOGGER_MINERALOIL             0x0301 /* White Mineral oil hazer                                     */
#define E120_PRODUCT_DETAIL_FOGGER_WATER                  0x0302 /* Water hazer                                                 */
#define E120_PRODUCT_DETAIL_C02                           0x0303 /* Dry Ice/Carbon Dioxide based                                */
#define E120_PRODUCT_DETAIL_LN2                           0x0304 /* Nitrogen based                                              */
#define E120_PRODUCT_DETAIL_BUBBLE                        0x0305 /* including foam                                              */
#define E120_PRODUCT_DETAIL_FLAME_PROPANE                 0x0306
#define E120_PRODUCT_DETAIL_FLAME_OTHER                   0x0307
#define E120_PRODUCT_DETAIL_OLEFACTORY_STIMULATOR         0x0308 /* Scents                                                      */
#define E120_PRODUCT_DETAIL_SNOW                          0x0309
#define E120_PRODUCT_DETAIL_WATER_JET                     0x030A /* Fountain controls etc                                       */
#define E120_PRODUCT_DETAIL_WIND                          0x030B /* Air Mover                                                   */
#define E120_PRODUCT_DETAIL_CONFETTI                      0x030C
#define E120_PRODUCT_DETAIL_HAZARD                        0x030D /* Any form of pyrotechnic control or device.                  */

/* Generally applied to Dimmers/Power controllers See Note 1                                                                    */
#define E120_PRODUCT_DETAIL_PHASE_CONTROL                 0x0400
#define E120_PRODUCT_DETAIL_REVERSE_PHASE_CONTROL         0x0401 /* includes FET/IGBT                                           */
#define E120_PRODUCT_DETAIL_SINE                          0x0402
#define E120_PRODUCT_DETAIL_PWM                           0x0403
#define E120_PRODUCT_DETAIL_DC                            0x0404 /* Variable voltage                                            */
#define E120_PRODUCT_DETAIL_HFBALLAST                     0x0405 /* for Fluroescent                                             */
#define E120_PRODUCT_DETAIL_HFHV_NEONBALLAST              0x0406 /* for Neon/Argon and other coldcathode.                       */
#define E120_PRODUCT_DETAIL_HFHV_EL                       0x0407 /* for Electroluminscent                                       */
#define E120_PRODUCT_DETAIL_MHR_BALLAST                   0x0408 /* for Metal Halide                                            */
#define E120_PRODUCT_DETAIL_BITANGLE_MODULATION           0x0409
#define E120_PRODUCT_DETAIL_FREQUENCY_MODULATION          0x040A
#define E120_PRODUCT_DETAIL_HIGHFREQUENCY_12V             0x040B /* as commonly used with MR16 lamps                            */
#define E120_PRODUCT_DETAIL_RELAY_MECHANICAL              0x040C /* See Note 1                                                  */
#define E120_PRODUCT_DETAIL_RELAY_ELECTRONIC              0x040D /* See Note 1, Note 2                                          */
#define E120_PRODUCT_DETAIL_SWITCH_ELECTRONIC             0x040E /* See Note 1, Note 2                                          */
#define E120_PRODUCT_DETAIL_CONTACTOR                     0x040F /* See Note 1                                                  */

/* Generally applied to Scenic drive                                                                                            */
#define E120_PRODUCT_DETAIL_MIRRORBALL_ROTATOR            0x0500
#define E120_PRODUCT_DETAIL_OTHER_ROTATOR                 0x0501 /* includes turntables                                         */
#define E120_PRODUCT_DETAIL_KABUKI_DROP                   0x0502
#define E120_PRODUCT_DETAIL_CURTAIN                       0x0503 /* flown or traveller                                          */
#define E120_PRODUCT_DETAIL_LINESET                       0x0504
#define E120_PRODUCT_DETAIL_MOTOR_CONTROL                 0x0505
#define E120_PRODUCT_DETAIL_DAMPER_CONTROL                0x0506 /* HVAC Damper                                                 */

/* Generally applied to Data Distribution                                                                                       */
#define E120_PRODUCT_DETAIL_SPLITTER                      0x0600 /* Includes buffers/repeaters                                  */
#define E120_PRODUCT_DETAIL_ETHERNET_NODE                 0x0601 /* DMX512 to/from Ethernet                                     */
#define E120_PRODUCT_DETAIL_MERGE                         0x0602 /* DMX512 combiner                                             */
#define E120_PRODUCT_DETAIL_DATAPATCH                     0x0603 /* Electronic Datalink Patch                                   */
#define E120_PRODUCT_DETAIL_WIRELESS_LINK                 0x0604 /* radio/infrared                                              */

/* Generally applied to Data Conversion and Interfaces                                                                          */
#define E120_PRODUCT_DETAIL_PROTOCOL_CONVERTOR            0x0701 /* D54/AMX192/Non DMX serial links, etc to/from DMX512         */
#define E120_PRODUCT_DETAIL_ANALOG_DEMULTIPLEX            0x0702 /* DMX to DC voltage                                           */
#define E120_PRODUCT_DETAIL_ANALOG_MULTIPLEX              0x0703 /* DC Voltage to DMX                                           */
#define E120_PRODUCT_DETAIL_SWITCH_PANEL                  0x0704 /* Pushbuttons to DMX or polled using RDM                      */

/* Generally applied to Audio or Video (AV) devices                                                                             */
#define E120_PRODUCT_DETAIL_ROUTER                        0x0800 /* Switching device                                            */
#define E120_PRODUCT_DETAIL_FADER                         0x0801 /* Single channel                                              */
#define E120_PRODUCT_DETAIL_MIXER                         0x0802 /* Multi-channel                                               */

/* Generally applied to Controllers, Backup devices and Test Equipment                                                          */
#define E120_PRODUCT_DETAIL_CHANGEOVER_MANUAL            0x0900 /* requires manual intervention to assume control of DMX line   */
#define E120_PRODUCT_DETAIL_CHANGEOVER_AUTO              0x0901 /* may automatically assume control of DMX line                 */
#define E120_PRODUCT_DETAIL_TEST                         0x0902 /* test equipment                                               */

/* Could be applied to any category                                                                                             */
#define E120_PRODUCT_DETAIL_GFI_RCD                      0x0A00 /* device includes GFI/RCD trip                                 */
#define E120_PRODUCT_DETAIL_BATTERY                      0x0A01 /* device is battery operated                                   */
#define E120_PRODUCT_DETAIL_CONTROLLABLE_BREAKER         0x0A02


#define E120_PRODUCT_DETAIL_OTHER                        0x7FFF /* for use where the Manufacturer believes that none of the
                                                                   defined details apply.                                            */
/* Manufacturer Specific Types                           0x8000-
                                                         0xDFFF                                                                 */

/* Note 1: Products intended for switching 50V AC / 120V DC or greater should be declared with a
           Product Category of PRODUCT_CATEGORY_POWER_CONTROL.

           Products only suitable for extra low voltage switching (typically up to 50VAC / 30VDC) at currents
           less than 1 ampere should be declared with a Product Category of PRODUCT_CATEGORY_DATA_CONVERSION.

           Please refer to GET: DEVICE_INFO and Table A-5 for an explanation of Product Category declaration.
   Note 2: Products with TTL, MOSFET or Open Collector Transistor Outputs or similar non-isolated electronic
           outputs should be declared as PRODUCT_DETAIL_SWITCH_ELECTRONIC. Use of PRODUCT_DETAIL_RELAY_ELECTRONIC
           shall be restricted to devices whereby the switched circuits are electrically isolated from the control signals.     */


/********************************************************/
/* Table A-7: Preset Playback Defines                   */
/********************************************************/

#define E120_PRESET_PLAYBACK_OFF                         0x0000 /* Returns to Normal DMX512 Input                               */
#define E120_PRESET_PLAYBACK_ALL                         0xFFFF /* Plays Scenes in Sequence if supported.                       */
/*      E120_PRESET_PLAYBACK_SCENE                       0x0001-
                                                         0xFFFE    Plays individual Scene #                                     */

/********************************************************/
/* Table A-8: Lamp State Defines                        */
/********************************************************/

#define E120_LAMP_OFF                                    0x00   /* No demonstrable light output                                 */
#define E120_LAMP_ON                                     0x01
#define E120_LAMP_STRIKE                                 0x02   /* Arc-Lamp ignite                                              */
#define E120_LAMP_STANDBY                                0x03   /* Arc-Lamp Reduced Power Mode                                  */
#define E120_LAMP_NOT_PRESENT                            0x04   /* Lamp not installed                                           */
#define E120_LAMP_ERROR                                  0x7F
/* Manufacturer-Specific States                          0x80-
                                                         0xDF                                                                   */

/********************************************************/
/* Table A-9: Lamp On Mode Defines                      */
/********************************************************/

#define E120_LAMP_ON_MODE_OFF                            0x00   /* Lamp Stays off until directly instructed to Strike.          */
#define E120_LAMP_ON_MODE_DMX                            0x01   /* Lamp Strikes upon receiving a DMX512 signal.                 */
#define E120_LAMP_ON_MODE_ON                             0x02   /* Lamp Strikes automatically at Power-up.                      */
#define E120_LAMP_ON_MODE_AFTER_CAL                      0x03   /* Lamp Strikes after Calibration or Homing procedure.          */
/* Manufacturer-Specific Modes                           0x80-
                                                         0xDF                                                                   */

/********************************************************/
/* Table A-10: Self Test Defines                        */
/********************************************************/

#define E120_SELF_TEST_OFF                               0x00   /* Turns Self Tests Off                                         */
/* Manufacturer Tests                                    0x01-
                                                         0xFE      Various Manufacturer Self Tests                              */
#define E120_SELF_TEST_ALL                               0xFF   /* Self Test All, if applicable                                 */

/********************************************************/
/* Table A-11: Power State Defines                      */
/********************************************************/

#define E120_POWER_STATE_FULL_OFF                        0x00   /* Completely disengages power to device. Device can no longer respond. */
#define E120_POWER_STATE_SHUTDOWN                        0x01   /* Reduced power mode, may require device reset to return to
                                                                   normal operation. Device still responds to messages.         */
#define E120_POWER_STATE_STANDBY                         0x02   /* Reduced power mode. Device can return to NORMAL without a
                                                                   reset. Device still responds to messages.                    */
#define E120_POWER_STATE_NORMAL                          0xFF   /* Normal Operating Mode.                                       */

/********************************************************/
/* Table A-12: Sensor Type Defines                      */
/********************************************************/

#define E120_SENS_TEMPERATURE                            0x00
#define E120_SENS_VOLTAGE                                0x01
#define E120_SENS_CURRENT                                0x02
#define E120_SENS_FREQUENCY                              0x03
#define E120_SENS_RESISTANCE                             0x04   /* Eg: Cable resistance                                         */
#define E120_SENS_POWER                                  0x05
#define E120_SENS_MASS                                   0x06   /* Eg: Truss load Cell                                          */
#define E120_SENS_LENGTH                                 0x07
#define E120_SENS_AREA                                   0x08
#define E120_SENS_VOLUME                                 0x09   /* Eg: Smoke Fluid                                              */
#define E120_SENS_DENSITY                                0x0A
#define E120_SENS_VELOCITY                               0x0B
#define E120_SENS_ACCELERATION                           0x0C
#define E120_SENS_FORCE                                  0x0D
#define E120_SENS_ENERGY                                 0x0E
#define E120_SENS_PRESSURE                               0x0F
#define E120_SENS_TIME                                   0x10
#define E120_SENS_ANGLE                                  0x11
#define E120_SENS_POSITION_X                             0x12   /* E.g.: Lamp position on Truss                                 */
#define E120_SENS_POSITION_Y                             0x13
#define E120_SENS_POSITION_Z                             0x14
#define E120_SENS_ANGULAR_VELOCITY                       0x15   /* E.g.: Wind speed                                             */
#define E120_SENS_LUMINOUS_INTENSITY                     0x16
#define E120_SENS_LUMINOUS_FLUX                          0x17
#define E120_SENS_ILLUMINANCE                            0x18
#define E120_SENS_CHROMINANCE_RED                        0x19
#define E120_SENS_CHROMINANCE_GREEN                      0x1A
#define E120_SENS_CHROMINANCE_BLUE                       0x1B
#define E120_SENS_CONTACTS                               0x1C   /* E.g.: Switch inputs.                                         */
#define E120_SENS_MEMORY                                 0x1D   /* E.g.: ROM Size                                               */
#define E120_SENS_ITEMS                                  0x1E   /* E.g.: Scroller gel frames.                                   */
#define E120_SENS_HUMIDITY                               0x1F
#define E120_SENS_COUNTER_16BIT                          0x20
#define E120_SENS_OTHER                                  0x7F
/* Manufacturer-Specific Sensors                         0x80-
                                                         0xFF                                                                   */

/********************************************************/
/* Table A-13: Sensor Unit Defines                      */
/********************************************************/

#define E120_UNITS_NONE                                  0x00   /* CONTACTS                                                     */
#define E120_UNITS_CENTIGRADE                            0x01   /* TEMPERATURE                                                  */
#define E120_UNITS_VOLTS_DC                              0x02   /* VOLTAGE                                                    */
#define E120_UNITS_VOLTS_AC_PEAK                         0x03   /* VOLTAGE                                                      */
#define E120_UNITS_VOLTS_AC_RMS                          0x04   /* VOLTAGE                                                      */
#define E120_UNITS_AMPERE_DC                             0x05   /* CURRENT                                                      */
#define E120_UNITS_AMPERE_AC_PEAK                        0x06   /* CURRENT                                                      */
#define E120_UNITS_AMPERE_AC_RMS                         0x07   /* CURRENT                                                      */
#define E120_UNITS_HERTZ                                 0x08   /* FREQUENCY / ANG_VEL                                          */
#define E120_UNITS_OHM                                   0x09   /* RESISTANCE                                             */
#define E120_UNITS_WATT                                  0x0A   /* POWER                                              */
#define E120_UNITS_KILOGRAM                              0x0B   /* MASS                                                         */
#define E120_UNITS_METERS                                0x0C   /* LENGTH / POSITION                                        */
#define E120_UNITS_METERS_SQUARED                        0x0D   /* AREA                                                 */
#define E120_UNITS_METERS_CUBED                          0x0E   /* VOLUME                                                       */
#define E120_UNITS_KILOGRAMMES_PER_METER_CUBED           0x0F   /* DENSITY                                                      */
#define E120_UNITS_METERS_PER_SECOND                     0x10   /* VELOCITY                                                   */
#define E120_UNITS_METERS_PER_SECOND_SQUARED             0x11   /* ACCELERATION                                                 */
#define E120_UNITS_NEWTON                                0x12   /* FORCE                                                        */
#define E120_UNITS_JOULE                                 0x13   /* ENERGY                                                   */
#define E120_UNITS_PASCAL                                0x14   /* PRESSURE                                                   */
#define E120_UNITS_SECOND                                0x15   /* TIME                                                         */
#define E120_UNITS_DEGREE                                0x16   /* ANGLE                                                  */
#define E120_UNITS_STERADIAN                             0x17   /* ANGLE                                                  */
#define E120_UNITS_CANDELA                               0x18   /* LUMINOUS_INTENSITY                                           */
#define E120_UNITS_LUMEN                                 0x19   /* LUMINOUS_FLUX                                            */
#define E120_UNITS_LUX                                   0x1A   /* ILLUMINANCE                                                */
#define E120_UNITS_IRE                                   0x1B   /* CHROMINANCE                                                  */
#define E120_UNITS_BYTE                                  0x1C   /* MEMORY                                                     */
/* Manufacturer-Specific Units                           0x80-
                                                 0xFF                                                           */


/********************************************************/
/* Table A-14: Sensor Unit Prefix Defines               */
/********************************************************/

#define E120_PREFIX_NONE                                 0x00   /* Multiply by 1                                                */
#define E120_PREFIX_DECI                                 0x01   /* Multiply by 10-1                                             */
#define E120_PREFIX_CENTI                                0x02   /* Multiply by 10-2                                             */
#define E120_PREFIX_MILLI                                0x03   /* Multiply by 10-3                                             */
#define E120_PREFIX_MICRO                                0x04   /* Multiply by 10-6                                             */
#define E120_PREFIX_NANO                                 0x05   /* Multiply by 10-9                                             */
#define E120_PREFIX_PICO                                 0x06   /* Multiply by 10-12                                          */
#define E120_PREFIX_FEMPTO                               0x07   /* Multiply by 10-15                                          */
#define E120_PREFIX_ATTO                                 0x08   /* Multiply by 10-18                                          */
#define E120_PREFIX_ZEPTO                                0x09   /* Multiply by 10-21                                          */
#define E120_PREFIX_YOCTO                                0x0A   /* Multiply by 10-24                                          */
#define E120_PREFIX_DECA                                 0x11   /* Multiply by 10+1                                             */
#define E120_PREFIX_HECTO                                0x12   /* Multiply by 10+2                                             */
#define E120_PREFIX_KILO                                 0x13   /* Multiply by 10+3                                             */
#define E120_PREFIX_MEGA                                 0x14   /* Multiply by 10+6                                             */
#define E120_PREFIX_GIGA                                 0x15   /* Multiply by 10+9                                             */
#define E120_PREFIX_TERRA                                0x16   /* Multiply by 10+12                                          */
#define E120_PREFIX_PETA                                 0x17   /* Multiply by 10+15                                          */
#define E120_PREFIX_EXA                                  0x18   /* Multiply by 10+18                                          */
#define E120_PREFIX_ZETTA                                0x19   /* Multiply by 10+21                                          */
#define E120_PREFIX_YOTTA                                0x1A   /* Multiply by 10+24                                          */


/********************************************************/
/* Table A-15: Data Type Defines                        */
/********************************************************/

#define E120_DS_NOT_DEFINED                              0x00   /* Data type is not defined                                     */
#define E120_DS_BIT_FIELD                                0x01   /* Data is bit packed                                     */
#define E120_DS_ASCII                                    0x02   /* Data is a string                                     */
#define E120_DS_UNSIGNED_BYTE                            0x03   /* Data is an array of unsigned bytes                           */
#define E120_DS_SIGNED_BYTE                              0x04   /* Data is an array of signed bytes                             */
#define E120_DS_UNSIGNED_WORD                            0x05   /* Data is an array of unsigned 16-bit words                  */
#define E120_DS_SIGNED_WORD                              0x06   /* Data is an array of signed 16-bit words                      */
#define E120_DS_UNSIGNED_DWORD                           0x07   /* Data is an array of unsigned 32-bit words                  */
#define E120_DS_SIGNED_DWORD                             0x08   /* Data is an array of signed 32-bit words            */
/* Manufacturer-Specific Data Types                0x80-                                                                  */
/*                                                       0xDF                                                                 */

/********************************************************/
/* Table A-16: Parameter Desc. Command Class Defines    */
/********************************************************/

#define E120_CC_GET                                      0x01   /* PID supports GET only                                        */
#define E120_CC_SET                                      0x02   /* PID supports SET only                                        */
#define E120_CC_GET_SET                                  0x03   /* PID supports GET & SET                                       */

/********************************************************/
/* Table A-17: Response NACK Reason Code Defines        */
/********************************************************/

#define E120_NR_UNKNOWN_PID                              0x0000 /* The responder cannot comply with request because the message
                                                                   is not implemented in responder.                             */
#define E120_NR_FORMAT_ERROR                             0x0001 /* The responder cannot interpret request as controller data
                                                                   was not formatted correctly.                                 */
#define E120_NR_HARDWARE_FAULT                           0x0002 /* The responder cannot comply due to an internal hardware fault*/
#define E120_NR_PROXY_REJECT                             0x0003 /* Proxy is not the RDM line master and cannot comply with message.*/
#define E120_NR_WRITE_PROTECT                            0x0004 /* SET Command normally allowed but being blocked currently.    */
#define E120_NR_UNSUPPORTED_COMMAND_CLASS                0x0005 /* Not valid for Command Class attempted. May be used where
                                                                   GET allowed but SET is not supported.                        */
#define E120_NR_DATA_OUT_OF_RANGE                        0x0006 /* Value for given Parameter out of allowable range or
                                                                   not supported.                                               */
#define E120_NR_BUFFER_FULL                              0x0007 /* Buffer or Queue space currently has no free space to store data. */
#define E120_NR_PACKET_SIZE_UNSUPPORTED                  0x0008 /* Incoming message exceeds buffer capacity.                    */
#define E120_NR_SUB_DEVICE_OUT_OF_RANGE                  0x0009 /* Sub-Device is out of range or unknown.                       */
#define E120_NR_PROXY_BUFFER_FULL                        0x000A /* Proxy buffer is full and can not store any more Queued       */
                                                                /* Message or Status Message responses.                         */

/********************************************************************************************************************************/
/********************************************************************************************************************************/
/* ANSI E1.37-1 DEFINES                                                                                                         */
/********************************************************************************************************************************/
/********************************************************************************************************************************/

/********************************************************/
/* E1.37-1 Table A-2: Preset Programmed Defines         */
/********************************************************/
#define E137_1_PRESET_NOT_PROGRAMMED                     0x00 /* Preset Scene not programmed.                                   */
#define E137_1_PRESET_PROGRAMMED                         0x01 /* Preset Scene programmed.                                       */
#define E137_1_PRESET_PROGRAMMED_READ_ONLY               0x02 /* Preset Scene read-only, factory programmed.                    */

/********************************************************/
/* E1.37-1 Table A-3: Merge Mode Defines                */
/********************************************************/
#define E137_1_MERGEMODE_DEFAULT                         0x00 /* Preset overrides DMX512 default behavior as defined in         */
                                                              /* E1.20 PRESET_PLAYBACK                                          */
#define E137_1_MERGEMODE_HTP                             0x01 /* Highest Takes Precedence on slot by slot basis                 */
#define E137_1_MERGEMODE_LTP                             0x02 /* Latest Takes Precedence from Preset or DMX512 on slot by slot  */
#define E137_1_MERGEMODE_DMX_ONLY                        0x03 /* DMX512 only, Preset ignored                                    */
#define E137_1_MERGEMODE_OTHER                           0xFF /* Other (undefined) merge mode                                   */
