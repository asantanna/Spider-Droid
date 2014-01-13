	TAGS	UNSORTED
MC_DEF_BAUD	Software/firmware/include/polulu_2s9v1.h	107	#define MC_DEF_BAUD   B38400
MC_CMD_SIGN	Software/firmware/include/polulu_2s9v1.h	513	#define MC_CMD_SIGN                 0xAA
MC_DEFAULT_DEVICE_NUM	Software/firmware/include/polulu_2s9v1.h	554	#define MC_DEFAULT_DEVICE_NUM       9
MC_CMD_GET_VERSION	Software/firmware/include/polulu_2s9v1.h	593	#define MC_CMD_GET_VERSION          1         // not used
MC_CMD_GET_ERR_BYTE	Software/firmware/include/polulu_2s9v1.h	651	#define MC_CMD_GET_ERR_BYTE         2         // not used
MC_CMD_GET_CONF	Software/firmware/include/polulu_2s9v1.h	709	#define MC_CMD_GET_CONF             3         // not used
MC_CMD_SET_CONF	Software/firmware/include/polulu_2s9v1.h	767	#define MC_CMD_SET_CONF             4         // configure
MC_CMD_UNKNOWN	Software/firmware/include/polulu_2s9v1.h	826	#define MC_CMD_UNKNOWN              5         // not doc
MC_CMD_COAST_M0	Software/firmware/include/polulu_2s9v1.h	883	#define MC_CMD_COAST_M0             6         // no resist motor 0
MC_CMD_COAST_M1	Software/firmware/include/polulu_2s9v1.h	950	#define MC_CMD_COAST_M1             7         // no resist motor 1
MC_CMD_FWD_M0	Software/firmware/include/polulu_2s9v1.h	1018	#define MC_CMD_FWD_M0               8         // set M0 speed (forward)
MC_CMD_FWD_8BIT_M0	Software/firmware/include/polulu_2s9v1.h	1090	#define MC_CMD_FWD_8BIT_M0          9         // set M0 8-bit speed+128 (forward)
MC_CMD_BCK_M0	Software/firmware/include/polulu_2s9v1.h	1172	#define MC_CMD_BCK_M0             0xA         // set M0 speed (backward)
MC_CMD_BCK_8BIT_M0	Software/firmware/include/polulu_2s9v1.h	1245	#define MC_CMD_BCK_8BIT_M0        0xB         // set M0 8-bit speed+128 (backward)
MC_CMD_FWD_M1	Software/firmware/include/polulu_2s9v1.h	1329	#define MC_CMD_FWD_M1             0xC         // set M1 speed (forward)
MC_CMD_FWD_8BIT_M1	Software/firmware/include/polulu_2s9v1.h	1401	#define MC_CMD_FWD_8BIT_M1        0xD         // set M1 8-bit speed+128 (forward)
MC_CMD_BCK_M1	Software/firmware/include/polulu_2s9v1.h	1483	#define MC_CMD_BCK_M1             0xE         // set M1 speed (backward)
MC_CMD_BCK_8BIT_M1	Software/firmware/include/polulu_2s9v1.h	1556	#define MC_CMD_BCK_8BIT_M1        0xF         // set M1 8-bit speed+128 (backward)
MC_SCONF_END_VAL_0	Software/firmware/include/polulu_2s9v1.h	2613	#define MC_SCONF_END_VAL_0          0x55      // end value 0
MC_SCONF_END_VAL_1	Software/firmware/include/polulu_2s9v1.h	2674	#define MC_SCONF_END_VAL_1          0x2A      // end value 1
MC_SCONF_PARAM_DEVICE_ID	Software/firmware/include/polulu_2s9v1.h	2736	#define MC_SCONF_PARAM_DEVICE_ID    0         // set motor speed
MC_SCONF_PARAM_PWM_MODE	Software/firmware/include/polulu_2s9v1.h	2801	#define MC_SCONF_PARAM_PWM_MODE     1         // set PWM mode
MC_SCONF_PARAM_ERR_MODE	Software/firmware/include/polulu_2s9v1.h	2863	#define MC_SCONF_PARAM_ERR_MODE     2         // set error mode
MC_SCONF_PARAM_TIMEOUT	Software/firmware/include/polulu_2s9v1.h	2927	#define MC_SCONF_PARAM_TIMEOUT      3         // set serial timeout
MC_SCONF_SPD_7BIT	Software/firmware/include/polulu_2s9v1.h	3014	#define MC_SCONF_SPD_7BIT           0         // use 7 bit motor speed - max is 0x7f
MC_SCONF_SPD_8BIT	Software/firmware/include/polulu_2s9v1.h	3099	#define MC_SCONF_SPD_8BIT           1         // use 8 bit motor speed - max is 0xff
MC_SCONF_HIGH_FREQ	Software/firmware/include/polulu_2s9v1.h	3184	#define MC_SCONF_HIGH_FREQ          0         // higher pitched noise - more power consumption
MC_SCONF_LOW_FREQ	Software/firmware/include/polulu_2s9v1.h	3279	#define MC_SCONF_LOW_FREQ           2         // lower/louder pitched noise - less power consumption
MC_SCONF_IGNORE_ERR	Software/firmware/include/polulu_2s9v1.h	3399	#define MC_SCONF_IGNORE_ERR         0         // ignore errors - keep running
MC_SCONF_STOP_ON_ERR	Software/firmware/include/polulu_2s9v1.h	3477	#define MC_SCONF_STOP_ON_ERR        1         // stop motors if error or timeout
MC_SCONF_TIMEOUT_DISABLE	Software/firmware/include/polulu_2s9v1.h	3576	#define MC_SCONF_TIMEOUT_DISABLE    0         // disable error on timeout
MC_SCONF_TO_MULT_MASK	Software/firmware/include/polulu_2s9v1.h	3650	#define MC_SCONF_TO_MULT_MASK       0x0F      // low 4 bits are "multiplier"
MC_SCONF_TO_SHIFT_MASK	Software/firmware/include/polulu_2s9v1.h	3727	#define MC_SCONF_TO_SHIFT_MASK      0x70      // next 3 bits are "shift"
GYRO_MAX_SPI_CLK	Software/firmware/include/plax_L3G4200D.h	76	#define GYRO_MAX_SPI_CLK          (10ul*1000ul*1000ul)
GYRO_SAFE_SPI_CLK	Software/firmware/include/plax_L3G4200D.h	154	#define GYRO_SAFE_SPI_CLK         ( 8ul*1000ul*1000ul)
GYRO_SPI_MODE	Software/firmware/include/plax_L3G4200D.h	571	#define GYRO_SPI_MODE             (SPI_CPHA | SPI_CPOL)
GYRO_SPI_BPW	Software/firmware/include/plax_L3G4200D.h	647	#define GYRO_SPI_BPW              8
GYRO_250DPS_MULT	Software/firmware/include/plax_L3G4200D.h	718	#define GYRO_250DPS_MULT          0.00875f
GYRO_500DPS_MULT	Software/firmware/include/plax_L3G4200D.h	761	#define GYRO_500DPS_MULT          0.0175f
GYRO_2000DPS_MULT	Software/firmware/include/plax_L3G4200D.h	803	#define GYRO_2000DPS_MULT         0.07f
GYRO_ADDR_READ	Software/firmware/include/plax_L3G4200D.h	886	#define GYRO_ADDR_READ            0x80
GYRO_ADDR_WRITE	Software/firmware/include/plax_L3G4200D.h	925	#define GYRO_ADDR_WRITE           0
GYRO_ADDR_AUTO_INC	Software/firmware/include/plax_L3G4200D.h	962	#define GYRO_ADDR_AUTO_INC        0x40
GYRO_ADDR_NO_INC	Software/firmware/include/plax_L3G4200D.h	1001	#define GYRO_ADDR_NO_INC          0
GYRO_ADDR_MASK	Software/firmware/include/plax_L3G4200D.h	1038	#define GYRO_ADDR_MASK            0x3F
GYRO_WHOAMI_ADDR	Software/firmware/include/plax_L3G4200D.h	1119	#define GYRO_WHOAMI_ADDR          0x0F
GYRO_WHOAMI_REPLY	Software/firmware/include/plax_L3G4200D.h	1158	#define GYRO_WHOAMI_REPLY         0xD3
GYRO_CR1_ADDR	Software/firmware/include/plax_L3G4200D.h	1212	#define GYRO_CR1_ADDR             0x20
GYRO_CR1_DR_MASK	Software/firmware/include/plax_L3G4200D.h	1252	#define GYRO_CR1_DR_MASK          0xC0
GYRO_CR1_DR_100HZ	Software/firmware/include/plax_L3G4200D.h	1292	#define GYRO_CR1_DR_100HZ         0x00  /* output data rate */
GYRO_CR1_DR_200HZ	Software/firmware/include/plax_L3G4200D.h	1355	#define GYRO_CR1_DR_200HZ         0x40
GYRO_CR1_DR_400HZ	Software/firmware/include/plax_L3G4200D.h	1394	#define GYRO_CR1_DR_400HZ         0x80
GYRO_CR1_DR_800HZ	Software/firmware/include/plax_L3G4200D.h	1433	#define GYRO_CR1_DR_800HZ         0xC0
GYRO_CR1_BW_MASK	Software/firmware/include/plax_L3G4200D.h	1473	#define GYRO_CR1_BW_MASK          0x30  /* cutoff freqs - see datasheet */
GYRO_CR1_BW_CO_1	Software/firmware/include/plax_L3G4200D.h	1548	#define GYRO_CR1_BW_CO_1          0x00
GYRO_CR1_BW_CO_2	Software/firmware/include/plax_L3G4200D.h	1587	#define GYRO_CR1_BW_CO_2          0x10
GYRO_CR1_BW_CO_3	Software/firmware/include/plax_L3G4200D.h	1626	#define GYRO_CR1_BW_CO_3          0x20
GYRO_CR1_BW_CO_4	Software/firmware/include/plax_L3G4200D.h	1665	#define GYRO_CR1_BW_CO_4          0x30
GYRO_CR1_MODE_PWRDOWN	Software/firmware/include/plax_L3G4200D.h	1705	#define GYRO_CR1_MODE_PWRDOWN     0
GYRO_CR1_MODE_NORMAL	Software/firmware/include/plax_L3G4200D.h	1741	#define GYRO_CR1_MODE_NORMAL      0x08
GYRO_CR1_Z_EN	Software/firmware/include/plax_L3G4200D.h	1780	#define GYRO_CR1_Z_EN             0x04
GYRO_CR1_Y_EN	Software/firmware/include/plax_L3G4200D.h	1819	#define GYRO_CR1_Y_EN             0x02
GYRO_CR1_X_EN	Software/firmware/include/plax_L3G4200D.h	1858	#define GYRO_CR1_X_EN             0x01
GYRO_CR2_ADDR	Software/firmware/include/plax_L3G4200D.h	1912	#define GYRO_CR2_ADDR             0x21
GYRO_CR2_RSVD_MASK	Software/firmware/include/plax_L3G4200D.h	1952	#define GYRO_CR2_RSVD_MASK        0xC0  /* these bits must not be changed */
GYRO_CR2_HPM_MASK	Software/firmware/include/plax_L3G4200D.h	2029	#define GYRO_CR2_HPM_MASK         0x30  /* see datasheet */
GYRO_CR2_HPCF_MASK	Software/firmware/include/plax_L3G4200D.h	2089	#define GYRO_CR2_HPCF_MASK        0x0F
GYRO_CR3_ADDR	Software/firmware/include/plax_L3G4200D.h	2143	#define GYRO_CR3_ADDR             0x22
GYRO_CR3_INT1_EN	Software/firmware/include/plax_L3G4200D.h	2183	#define GYRO_CR3_INT1_EN          0x80
GYRO_CR3_INT1_BOOT_EN	Software/firmware/include/plax_L3G4200D.h	2222	#define GYRO_CR3_INT1_BOOT_EN     0x40
GYRO_CR3_INT1_LOW	Software/firmware/include/plax_L3G4200D.h	2261	#define GYRO_CR3_INT1_LOW         0x20
GYRO_CR3_OPEN_DRAIN	Software/firmware/include/plax_L3G4200D.h	2300	#define GYRO_CR3_OPEN_DRAIN       0x10
GYRO_CR3_INT2_DRDY_EN	Software/firmware/include/plax_L3G4200D.h	2339	#define GYRO_CR3_INT2_DRDY_EN     0x08
GYRO_CR3_INT2_FWI_EN	Software/firmware/include/plax_L3G4200D.h	2378	#define GYRO_CR3_INT2_FWI_EN      0x04
GYRO_CR3_INT2_OI_EN	Software/firmware/include/plax_L3G4200D.h	2417	#define GYRO_CR3_INT2_OI_EN       0x02
GYRO_CR3_INT2_FEMPTY_EN	Software/firmware/include/plax_L3G4200D.h	2456	#define GYRO_CR3_INT2_FEMPTY_EN   0x01
GYRO_CR4_ADDR	Software/firmware/include/plax_L3G4200D.h	2511	#define GYRO_CR4_ADDR             0x23
GYRO_CR4_BDU_EN	Software/firmware/include/plax_L3G4200D.h	2551	#define GYRO_CR4_BDU_EN           0x80
GYRO_CR4_BIG_ENDIAN	Software/firmware/include/plax_L3G4200D.h	2590	#define GYRO_CR4_BIG_ENDIAN       0x40
GYRO_CR4_LITTLE_ENDIAN	Software/firmware/include/plax_L3G4200D.h	2629	#define GYRO_CR4_LITTLE_ENDIAN    0
GYRO_CR4_FS_MASK	Software/firmware/include/plax_L3G4200D.h	2666	#define GYRO_CR4_FS_MASK          0x30
GYRO_CR4_FS_250DPS	Software/firmware/include/plax_L3G4200D.h	2705	#define GYRO_CR4_FS_250DPS        0x00
GYRO_CR4_FS_500DPS	Software/firmware/include/plax_L3G4200D.h	2744	#define GYRO_CR4_FS_500DPS        0x10
GYRO_CR4_FS_2000DPS	Software/firmware/include/plax_L3G4200D.h	2783	#define GYRO_CR4_FS_2000DPS       0x20
GYRO_CR4_STEST_MASK	Software/firmware/include/plax_L3G4200D.h	2823	#define GYRO_CR4_STEST_MASK       0x06
GYRO_CR4_SPI_3WIRE	Software/firmware/include/plax_L3G4200D.h	2862	#define GYRO_CR4_SPI_3WIRE        0x01
GYRO_CR5_ADDR	Software/firmware/include/plax_L3G4200D.h	2916	#define GYRO_CR5_ADDR             0x24
GYRO_CR5_REBOOT	Software/firmware/include/plax_L3G4200D.h	2956	#define GYRO_CR5_REBOOT           0x80
GYRO_CR5_FIFO_EN	Software/firmware/include/plax_L3G4200D.h	2995	#define GYRO_CR5_FIFO_EN          0x40
GYRO_CR5_HP_EN	Software/firmware/include/plax_L3G4200D.h	3034	#define GYRO_CR5_HP_EN            0x10
GYRO_CR5_INT1_SEL_MASK	Software/firmware/include/plax_L3G4200D.h	3073	#define GYRO_CR5_INT1_SEL_MASK    0x0C
GYRO_CR5_OUT_SEL_MASK	Software/firmware/include/plax_L3G4200D.h	3112	#define GYRO_CR5_OUT_SEL_MASK     0x03
GYRO_REF_ADDR	Software/firmware/include/plax_L3G4200D.h	3178	#define GYRO_REF_ADDR             0x25
GYRO_TEMP_ADDR	Software/firmware/include/plax_L3G4200D.h	3231	#define GYRO_TEMP_ADDR            0x26
GYRO_STATUS_ADDR	Software/firmware/include/plax_L3G4200D.h	3286	#define GYRO_STATUS_ADDR          0x27
GYRO_STATUS_OVER	Software/firmware/include/plax_L3G4200D.h	3326	#define GYRO_STATUS_OVER          0x80
GYRO_STATUS_Z_OVER	Software/firmware/include/plax_L3G4200D.h	3365	#define GYRO_STATUS_Z_OVER        0x40
GYRO_STATUS_Y_OVER	Software/firmware/include/plax_L3G4200D.h	3404	#define GYRO_STATUS_Y_OVER        0x20
GYRO_STATUS_X_OVER	Software/firmware/include/plax_L3G4200D.h	3443	#define GYRO_STATUS_X_OVER        0x10
GYRO_STATUS_AVAIL	Software/firmware/include/plax_L3G4200D.h	3482	#define GYRO_STATUS_AVAIL         0x08
GYRO_STATUS_Z_AVAIL	Software/firmware/include/plax_L3G4200D.h	3521	#define GYRO_STATUS_Z_AVAIL       0x04
GYRO_STATUS_Y_AVAIL	Software/firmware/include/plax_L3G4200D.h	3560	#define GYRO_STATUS_Y_AVAIL       0x02
GYRO_STATUS_X_AVAIL	Software/firmware/include/plax_L3G4200D.h	3599	#define GYRO_STATUS_X_AVAIL       0x01
GYRO_XL_ADDR	Software/firmware/include/plax_L3G4200D.h	3660	#define GYRO_XL_ADDR              0x28
GYRO_XH_ADDR	Software/firmware/include/plax_L3G4200D.h	3699	#define GYRO_XH_ADDR              0x29
GYRO_YL_ADDR	Software/firmware/include/plax_L3G4200D.h	3760	#define GYRO_YL_ADDR              0x2A
GYRO_YH_ADDR	Software/firmware/include/plax_L3G4200D.h	3799	#define GYRO_YH_ADDR              0x2B
GYRO_ZL_ADDR	Software/firmware/include/plax_L3G4200D.h	3860	#define GYRO_ZL_ADDR              0x2C
GYRO_ZH_ADDR	Software/firmware/include/plax_L3G4200D.h	3899	#define GYRO_ZH_ADDR              0x2D
GYRO_FIFO_ADDR	Software/firmware/include/plax_L3G4200D.h	3957	#define GYRO_FIFO_ADDR            0x2E
GYRO_FIFO_MODE_BYPASS	Software/firmware/include/plax_L3G4200D.h	3997	#define GYRO_FIFO_MODE_BYPASS     0x00
GYRO_FIFO_MODE_FIFO	Software/firmware/include/plax_L3G4200D.h	4036	#define GYRO_FIFO_MODE_FIFO       0x20
GYRO_FIFO_MODE_STREAM	Software/firmware/include/plax_L3G4200D.h	4075	#define GYRO_FIFO_MODE_STREAM     0x40
GYRO_FIFO_MODE_STF	Software/firmware/include/plax_L3G4200D.h	4114	#define GYRO_FIFO_MODE_STF        0x60
GYRO_FIFO_MODE_BTS	Software/firmware/include/plax_L3G4200D.h	4153	#define GYRO_FIFO_MODE_BTS        0x80
GYRO_FIFO_WTM_MASK	Software/firmware/include/plax_L3G4200D.h	4192	#define GYRO_FIFO_WTM_MASK        0x1F
GYRO_FIFO_SRC_ADDR	Software/firmware/include/plax_L3G4200D.h	4249	#define GYRO_FIFO_SRC_ADDR        0x2F
GYRO_FIFO_SRC_WTM	Software/firmware/include/plax_L3G4200D.h	4289	#define GYRO_FIFO_SRC_WTM         0x80
GYRO_FIFO_SRC_OVRN	Software/firmware/include/plax_L3G4200D.h	4328	#define GYRO_FIFO_SRC_OVRN        0x40
GYRO_FIFO_SRC_EMPTY	Software/firmware/include/plax_L3G4200D.h	4367	#define GYRO_FIFO_SRC_EMPTY       0x20
GYRO_FIFO_SRC_COUNT_MASK	Software/firmware/include/plax_L3G4200D.h	4406	#define GYRO_FIFO_SRC_COUNT_MASK  0x1F
GYRO_RSRC_ADDR	Software/firmware/include/plax_L3G4200D.h	4459	#define GYRO_RSRC_ADDR            0x2F
PHI_VERSION	Software/firmware/include/phi_defs.h	42	#define PHI_VERSION     "0.1 dev"
LOGFILE_NAME	Software/firmware/include/phi_defs.h	88	#define LOGFILE_NAME    "/var/log/phi.log"
VERBOSE_LOG	Software/firmware/include/phi_defs.h	131	#define VERBOSE_LOG     1
INT8	Software/firmware/include/phi_defs.h	173	#define INT8            char
INT16	Software/firmware/include/phi_defs.h	202	#define INT16           short
INT32	Software/firmware/include/phi_defs.h	232	#define INT32           long
INT64	Software/firmware/include/phi_defs.h	261	#define INT64           long long
UINT	Software/firmware/include/phi_defs.h	313	#define UINT            unsigned int
UINT8	Software/firmware/include/phi_defs.h	350	#define UINT8           unsigned char
UINT16	Software/firmware/include/phi_defs.h	388	#define UINT16          unsigned short
UINT32	Software/firmware/include/phi_defs.h	427	#define UINT32          unsigned long
UINT64	Software/firmware/include/phi_defs.h	465	#define UINT64          unsigned long long
BYTE	Software/firmware/include/phi_defs.h	533	#define BYTE            UINT8
WORD	Software/firmware/include/phi_defs.h	563	#define WORD            UINT16
DWORD	Software/firmware/include/phi_defs.h	594	#define DWORD           UINT32
BOOL	Software/firmware/include/phi_defs.h	625	#define BOOL            UINT8
FALSE	Software/firmware/include/phi_defs.h	656	#define FALSE           0
TRUE	Software/firmware/include/phi_defs.h	682	#define TRUE            1
COUNTOF	Software/firmware/include/phi_defs.h	709	#define COUNTOF(arr)    ((int) (sizeof(arr) / sizeof(arr[0])) )
PHI_ALLOC	Software/firmware/include/phi_defs.h	884	#define PHI_ALLOC(D)    ((D*)phi_allocHelper(sizeof(D)))
PHI_ALLOC_N	Software/firmware/include/phi_defs.h	941	#define PHI_ALLOC_N(n)  phi_allocHelper(n)
PHI_FREE	Software/firmware/include/phi_defs.h	984	#define PHI_FREE(p)     {free(p); p = NULL; }
PHI_ZERO	Software/firmware/include/phi_defs.h	1031	#define PHI_ZERO(v)     memset(&(v), 0, sizeof(v))
PHI_ZERO_P	Software/firmware/include/phi_defs.h	1082	#define PHI_ZERO_P(p,l) memset(p, 0, l)
BASH_BLACK	Software/firmware/include/phi_defs.h	1140	#define BASH_BLACK      "30"
BASH_RED	Software/firmware/include/phi_defs.h	1169	#define BASH_RED        "31"
BASH_GREEN	Software/firmware/include/phi_defs.h	1198	#define BASH_GREEN      "32"
BASH_YELLOW	Software/firmware/include/phi_defs.h	1227	#define BASH_YELLOW     "33"
BASH_BLUE	Software/firmware/include/phi_defs.h	1256	#define BASH_BLUE       "34"
BASH_MAGENTA	Software/firmware/include/phi_defs.h	1285	#define BASH_MAGENTA    "35"
BASH_CYAN	Software/firmware/include/phi_defs.h	1314	#define BASH_CYAN       "36"
BASH_WHITE	Software/firmware/include/phi_defs.h	1343	#define BASH_WHITE      "37"
BASH_START_COLOR	Software/firmware/include/phi_defs.h	1373	#define BASH_START_COLOR(c)   "\e[1;" c "m"
BASH_STOP_COLOR	Software/firmware/include/phi_defs.h	1417	#define BASH_STOP_COLOR       "\e[0m"
DO_PRAGMA	Software/firmware/include/phi_defs.h	1456	#define DO_PRAGMA(x)    _Pragma (#x)
INFO	Software/firmware/include/phi_defs.h	1493	#define INFO(m)         DO_PRAGMA(message ("\n" BASH_START_COLOR(BASH_GREEN)  "INFO - " m BASH_STOP_COLOR))
TODO	Software/firmware/include/phi_defs.h	1601	#define TODO(m)         DO_PRAGMA(message ("\n" BASH_START_COLOR(BASH_CYAN)  "TODO - " m BASH_STOP_COLOR))
WARN	Software/firmware/include/phi_defs.h	1708	#define WARN(m)         DO_PRAGMA(message ("\n" BASH_START_COLOR(BASH_YELLOW)    "WARNING - " m BASH_STOP_COLOR))
HACK	Software/firmware/include/phi_defs.h	1822	#define HACK(m)         DO_PRAGMA(message ("\n" BASH_START_COLOR(BASH_RED) "HACK - " m BASH_STOP_COLOR))
U64_1E6	Software/firmware/include/phi_defs.h	1937	#define U64_1E6         ((UINT64)1e6)
TV_TO_USEC	Software/firmware/include/phi_defs.h	1975	#define TV_TO_USEC(tv)  ( (((UINT64)tv.tv_sec) * U64_1E6) + ((UINT64)tv.tv_usec) )
LOG_INFO	Software/firmware/include/phi_defs.h	2071	#define LOG_INFO(...)   phi_logMsg("", __VA_ARGS__)
LOG_ERR	Software/firmware/include/phi_defs.h	2123	#define LOG_ERR(...)    phi_logMsg("ERROR: ", __VA_ARGS__)
LOG_FATAL	Software/firmware/include/phi_defs.h	2182	#define LOG_FATAL(...)  { printf("FATAL: "); printf(__VA_ARGS__); printf("\n"); phi_logMsg("FATAL: ", __VA_ARGS__); phi_abortProcess(-1); }
STR	Software/firmware/include/phi_defs.h	2348	#define STR(s)                #s              /* convert to string */
STR_MACRO	Software/firmware/include/phi_defs.h	2418	#define STR_MACRO(m)          STR(m)          /* convert to string (version for use inside a macro - requires two steps */
Q	Software/firmware/include/phi_defs.h	2541	#define Q(s)                  "\"" #s "\""    /* convert to quoted string */
UART_DRIVER_NAME	Software/firmware/include/phi_defs.h	2637	#define UART_DRIVER_NAME  "/dev/ttyAMA0"
SPI0_DRIVER_NAME	Software/firmware/include/phi_defs.h	2678	#define SPI0_DRIVER_NAME  "/dev/spidev0.0"
SPI1_DRIVER_NAME	Software/firmware/include/phi_defs.h	2721	#define SPI1_DRIVER_NAME  "/dev/spidev0.1"
OFF	Software/firmware/include/phi_defs.h	2791	    OFF,
RED	Software/firmware/include/phi_defs.h	2800	    RED,
FLASH_RED	Software/firmware/include/phi_defs.h	2809	    FLASH_RED,
YELLOW	Software/firmware/include/phi_defs.h	2824	    YELLOW,
FLASH_YELLOW	Software/firmware/include/phi_defs.h	2836	    FLASH_YELLOW,
GREEN	Software/firmware/include/phi_defs.h	2854	    GREEN,
FLASH_GREEN	Software/firmware/include/phi_defs.h	2865	    FLASH_GREEN
PHI_LED_COLOR	Software/firmware/include/phi_defs.h	2884	} PHI_LED_COLOR;
CAM_WIDTH	Software/firmware/include/phi_defs.h	2971	#define CAM_WIDTH         160
CAM_HEIGHT	Software/firmware/include/phi_defs.h	3001	#define CAM_HEIGHT        120
CAM_NUM_BYTES	Software/firmware/include/phi_defs.h	3031	#define CAM_NUM_BYTES     (CAM_WIDTH * CAM_HEIGHT)
DEF_PHILINK_PORT	Software/firmware/include/phi_defs.h	3094	#define DEF_PHILINK_PORT  1122
GYRO_SPI_IDX	Software/firmware/include/phi_defs.h	3149	#define GYRO_SPI_IDX      0
ADC_SPI_IDX	Software/firmware/include/phi_defs.h	3200	#define ADC_SPI_IDX       1
PHI_MUTEX_VAR_TYPE	Software/firmware/include/phi_defs.h	3247	#define PHI_MUTEX_VAR_TYPE          pthread_mutex_t
PHI_MUTEX_INITVAL	Software/firmware/include/phi_defs.h	3299	#define PHI_MUTEX_INITVAL           PTHREAD_MUTEX_INITIALIZER
PHI_MUTEX_DECL	Software/firmware/include/phi_defs.h	3361	#define PHI_MUTEX_DECL(name)        PHI_MUTEX_VAR_TYPE name = PHI_MUTEX_INITVAL
PHI_MUTEX_GET	Software/firmware/include/phi_defs.h	3441	#define PHI_MUTEX_GET(pSem)         pthread_mutex_lock(pSem)
PHI_MUTEX_RELEASE	Software/firmware/include/phi_defs.h	3502	#define PHI_MUTEX_RELEASE(pSem)     pthread_mutex_unlock(pSem)
PHI_MUTEX_TEST	Software/firmware/include/phi_defs.h	3565	#define PHI_MUTEX_TEST(pSem)        pthread_mutex_trylock(pSem)
LINK_OFF	Software/firmware/include/philink.h	43	    LINK_OFF,
LINK_STARTED	Software/firmware/include/philink.h	57	    LINK_STARTED,
LINK_CONNECTING	Software/firmware/include/philink.h	75	    LINK_CONNECTING,
LINK_CONNECTED	Software/firmware/include/philink.h	96	    LINK_CONNECTED,
LINK_ERROR	Software/firmware/include/philink.h	116	    LINK_ERROR,
LINK_CLOSED	Software/firmware/include/philink.h	132	    LINK_CLOSED
PHILINK_STATE	Software/firmware/include/philink.h	147	} PHILINK_STATE;
sock	Software/firmware/include/philink.h	186	  int sock;
ipAddr	Software/firmware/include/philink.h	204	  in_addr_t ipAddr;
port	Software/firmware/include/philink.h	223	  uint16_t port;
PHILINK_ARGS	Software/firmware/include/philink.h	232	} PHILINK_ARGS;
NUM_SIGN_BYTES	Software/firmware/include/philink.h	273	#define NUM_SIGN_BYTES          4
CMDP_SIGN	Software/firmware/include/philink.h	308	#define CMDP_SIGN               "CPV1"
CMDP_MOTOR_ELEM	Software/firmware/include/philink.h	347	#define CMDP_MOTOR_ELEM         NUM_MOTORS
PHI_CMD_PACKET	Software/firmware/include/philink.h	501	} PHI_CMD_PACKET;
STAP_SIGN	Software/firmware/include/philink.h	526	#define STAP_SIGN               "SPV1"
STAP_NUM_IMAGE_BYTES	Software/firmware/include/philink.h	565	#define STAP_NUM_IMAGE_BYTES    CAM_NUM_BYTES
STAP_NUM_GYRO_ELEM	Software/firmware/include/philink.h	611	#define STAP_NUM_GYRO_ELEM      3
STAP_NUM_JOINT_ELEM	Software/firmware/include/philink.h	645	#define STAP_NUM_JOINT_ELEM     NUM_MOTORS
PHI_STATE_PACKET	Software/firmware/include/philink.h	908	} PHI_STATE_PACKET;
_REENTRANT	Software/firmware/include/phi.h	103	#define _REENTRANT 1
ACCEL_I2C_CLK	Software/firmware/include/osepp_adxl345.h	66	#define ACCEL_I2C_CLK             ( 400ul*1000ul )
GYRO_SAFE_SPI_CLK	Software/firmware/include/osepp_adxl345.h	140	#define GYRO_SAFE_SPI_CLK         ( 8ul*1000ul*1000ul)
NUM_MOTORS	Software/firmware/include/motor.h	965	#define NUM_MOTORS  (4*3)
MOTOR_NAME_TO_IDX	Software/firmware/include/motor.h	992	#define MOTOR_NAME_TO_IDX(n) \
controllerId	Software/firmware/include/motor.h	1195	  BYTE controllerId;  // id of controller for motor
motorIdx	Software/firmware/include/motor.h	1247	  BYTE motorIdx;      // idx of motor in controller (0 or 1)
MOTOR_DEF	Software/firmware/include/motor.h	1306	} MOTOR_DEF;
ADC_MIN_SPI_CLK	Software/firmware/include/mcp3008.h	364	#define ADC_MIN_SPI_CLK         (10ul   * 1000ul)
ADC_MAX_SPI_CLK	Software/firmware/include/mcp3008.h	414	#define ADC_MAX_SPI_CLK         (1350ul * 1000ul)
ADC_SAFE_SPI_CLK	Software/firmware/include/mcp3008.h	465	#define ADC_SAFE_SPI_CLK        (1000ul * 1000ul)
ADC_SPI_MODE	Software/firmware/include/mcp3008.h	929	#define ADC_SPI_MODE            (SPI_CPHA | SPI_CPOL)
ADC_SPI_BPW	Software/firmware/include/mcp3008.h	1003	#define ADC_SPI_BPW             8
ADC_RAW_TO_VOLTS	Software/firmware/include/mcp3008.h	1078	#define ADC_RAW_TO_VOLTS(val, v_ref)      ( ((float)(val)) * ((float)(v_ref)) / 1024.0f )
ADC_CMD1_START	Software/firmware/include/mcp3008.h	1400	#define ADC_CMD1_START          0x01      // this is the "start bit"
ADC_CMD2_SINGLE	Software/firmware/include/mcp3008.h	1513	#define ADC_CMD2_SINGLE         0x80      // use single ended inputs (8x)
ADC_CMD2_DIFF	Software/firmware/include/mcp3008.h	1587	#define ADC_CMD2_DIFF           0x00      // use differential inputs (4x)
ADC_CMD2_ADDR_MASK	Software/firmware/include/mcp3008.h	1661	#define ADC_CMD2_ADDR_MASK      0x70      // address mask (bits = [0,7])
ADC_DATA2_NOT_USED_MASK	Software/firmware/include/mcp3008.h	1735	#define ADC_DATA2_NOT_USED_MASK 0x08      // this bit is not used
ADC_DATA2_NULL_MASK	Software/firmware/include/mcp3008.h	1801	#define ADC_DATA2_NULL_MASK     0x04      // this bit will be zero
ADC_DATA2_MASK	Software/firmware/include/mcp3008.h	1868	#define ADC_DATA2_MASK          0x03      // these bits are the result's high bits
ADC_DATA3_MASK	Software/firmware/include/mcp3008.h	1974	#define ADC_DATA3_MASK          0xFF      // these bits are the result's low bits
__JSMN_H_	Software/firmware/include/jsmn.h	26	#define __JSMN_H_
JSMN_PRIMITIVE	Software/firmware/include/jsmn.h	201		JSMN_PRIMITIVE = 0,
JSMN_OBJECT	Software/firmware/include/jsmn.h	222		JSMN_OBJECT = 1,
JSMN_ARRAY	Software/firmware/include/jsmn.h	240		JSMN_ARRAY = 2,
JSMN_STRING	Software/firmware/include/jsmn.h	257		JSMN_STRING = 3
jsmntype_t	Software/firmware/include/jsmn.h	275	} jsmntype_t;
JSMN_ERROR_NOMEM	Software/firmware/include/jsmn.h	343		JSMN_ERROR_NOMEM = -1,
JSMN_ERROR_INVAL	Software/firmware/include/jsmn.h	411		JSMN_ERROR_INVAL = -2,
JSMN_ERROR_PART	Software/firmware/include/jsmn.h	500		JSMN_ERROR_PART = -3,
JSMN_SUCCESS	Software/firmware/include/jsmn.h	550		JSMN_SUCCESS = 0
jsmnerr_t	Software/firmware/include/jsmn.h	569	} jsmnerr_t;
type	Software/firmware/include/jsmn.h	796		jsmntype_t type;
start	Software/firmware/include/jsmn.h	807		int start;
end	Software/firmware/include/jsmn.h	819		int end;
size	Software/firmware/include/jsmn.h	829		int size;
parent	Software/firmware/include/jsmn.h	865		int parent;
jsmntok_t	Software/firmware/include/jsmn.h	882	} jsmntok_t;
pos	Software/firmware/include/jsmn.h	1073		unsigned int pos; /* offset in the JSON string */
toknext	Software/firmware/include/jsmn.h	1115		int toknext; /* next token to allocate */
toksuper	Software/firmware/include/jsmn.h	1158		int toksuper; /* superior token node, e.g parent object or array */
jsmn_parser	Software/firmware/include/jsmn.h	1224	} jsmn_parser;
halFunc_void	Software/firmware/include/hal.h	54	typedef void   (*halFunc_void)   ();
halFunc_int	Software/firmware/include/hal.h	91	typedef int    (*halFunc_int)    ();
halFunc_pChar	Software/firmware/include/hal.h	128	typedef char*  (*halFunc_pChar)  ();
halFunc_BOOL	Software/firmware/include/hal.h	166	typedef BOOL   (*halFunc_BOOL)   ();
halFunc_INT8	Software/firmware/include/hal.h	203	typedef INT8   (*halFunc_INT8)   ();
halFunc_UINT8	Software/firmware/include/hal.h	240	typedef UINT8  (*halFunc_UINT8)  ();
halFunc_UINT16	Software/firmware/include/hal.h	277	typedef UINT16 (*halFunc_UINT16) ();
pName	Software/firmware/include/hal.h	340	  const char* pName;
initPeripherals	Software/firmware/include/hal.h	374	  halFunc_pChar initPeripherals;
gyroInit	Software/firmware/include/hal.h	422	  halFunc_BOOL gyroInit;
gyroGetDeltas	Software/firmware/include/hal.h	447	  halFunc_void gyroGetDeltas;
gyroGetTemp	Software/firmware/include/hal.h	477	  halFunc_INT8 gyroGetTemp;
getJointPosition	Software/firmware/include/hal.h	527	  halFunc_UINT16 getJointPosition;
setMotorPower	Software/firmware/include/hal.h	562	  halFunc_void   setMotorPower;
setControllerId	Software/firmware/include/hal.h	594	  halFunc_void   setControllerId;
HAL_FUNCS	Software/firmware/include/hal.h	614	} HAL_FUNCS;
HAL_name	Software/firmware/include/hal.h	707	#define HAL_name              (g_pHal   -> pName)
HAL_initPeripherals	Software/firmware/include/hal.h	757	#define HAL_initPeripherals   (*(g_pHal -> initPeripherals))
HAL_gyroInit	Software/firmware/include/hal.h	819	#define HAL_gyroInit          (*(g_pHal -> gyroInit))
HAL_gyroGetDeltas	Software/firmware/include/hal.h	873	#define HAL_gyroGetDeltas     (*(g_pHal -> gyroGetDeltas))
HAL_gyroGetTemp	Software/firmware/include/hal.h	932	#define HAL_gyroGetTemp       (*(g_pHal -> gyroGetTemp))
HAL_getJointPosition	Software/firmware/include/hal.h	990	#define HAL_getJointPosition  (*(g_pHal -> getJointPosition))
HAL_setMotorPower	Software/firmware/include/hal.h	1052	#define HAL_setMotorPower     (*(g_pHal -> setMotorPower))
HAL_setControllerId	Software/firmware/include/hal.h	1111	#define HAL_setControllerId   (*(g_pHal -> setControllerId))
INIT	Software/firmware/include/globals.h	77	#define INIT(v)   = v
EXT	Software/firmware/include/globals.h	99	#define EXT       // nothing
INIT	Software/firmware/include/globals.h	134	#define INIT(v)   // nothing
EXT	Software/firmware/include/globals.h	163	#define EXT       extern
g_uname	Software/firmware/include/globals.h	384	EXT struct utsname g_uname;              // can't init this to zero here for some reason
MAX_JSON_TOKENS	Software/firmware/src/webjson.c	913	#define MAX_JSON_TOKENS       256
PARSER_NUM_TOK	Software/firmware/src/webjson.c	979	#define PARSER_NUM_TOK(parser)  ((parser).toknext)
TOK_START	Software/firmware/src/webjson.c	1031	#define TOK_START(pTok)       (pJsonReq + (pTok)->start)
TOK_LEN	Software/firmware/src/webjson.c	1088	#define TOK_LEN(pTok)         ((pTok)->end - (pTok)->start)
TOK_TYPE	Software/firmware/src/webjson.c	1148	#define TOK_TYPE(pTok)        ((pTok)->type)
TOK_NUM_CHILD	Software/firmware/src/webjson.c	1193	#define TOK_NUM_CHILD(pTok)   ((pTok)->size)
TOK_EQ	Software/firmware/src/webjson.c	1239	#define TOK_EQ(pTok,s) \
_jsonCmdReply_t	Software/firmware/src/webjson.c	1404	typedef struct _jsonCmdReply_t {
_jsonCmdReply_t::pReply	Software/firmware/src/webjson.c	1430	  char* pReply;
_jsonCmdReply_t::pNext	Software/firmware/src/webjson.c	1464	  struct _jsonCmdReply_t* pNext;
PHI_JSON_CMD_REPLY_TYPE	Software/firmware/src/webjson.c	1474	} PHI_JSON_CMD_REPLY_TYPE;
jsonCmdHandler	Software/firmware/src/webjson.c	1535	typedef PHI_JSON_CMD_REPLY_TYPE* (*jsonCmdHandler)(jsmntok_t** ppTok, char* pJsonReq);
JSON_HANDLER	Software/firmware/src/webjson.c	1707	#define JSON_HANDLER(h) PHI_JSON_CMD_REPLY_TYPE* json_##h (jsmntok_t** ppTok, char* pJsonReq)
pCmd	Software/firmware/src/webjson.c	2192	  char *pCmd;
handler	Software/firmware/src/webjson.c	2215	  jsonCmdHandler handler;
PHI_JSON_CMD_TYPE	Software/firmware/src/webjson.c	2229	} PHI_JSON_CMD_TYPE;
CMD_ENTRY	Software/firmware/src/webjson.c	2257	#define CMD_ENTRY(c) { STR(c) , json_ ## c }
validCmds	Software/firmware/src/webjson.c	2313	PHI_JSON_CMD_TYPE validCmds[] = {
jsonReply_start	Software/firmware/src/webjson.c	2745	char jsonReply_start[] = "{" Q(reply) ": [\n";
jsonReply_end	Software/firmware/src/webjson.c	2792	char jsonReply_end[]   = "] }";
jsonReplyObject_start	Software/firmware/src/webjson.c	2892	char jsonReplyObject_start[] = "{\n" Q(cmd) ":" Q(%s) ",\n";
jsonReplyObject_end	Software/firmware/src/webjson.c	2953	char jsonReplyObject_end[]   = "}";
jsonParseError	Software/firmware/src/webjson.c	3063	char jsonParseError[] = "{ " Q(error) ":" Q(PHI could not process your JSON request.  It''s probably your fault btw.) " }\n";
phi_processJson	Software/firmware/src/webjson.c	3344	char* phi_processJson(char *pJsonReq) {
phi_freeJsonReply	Software/firmware/src/webjson.c	8099	void phi_freeJsonReply(char* pJsonReply) {
getJsonHandler	Software/firmware/src/webjson.c	8179	jsonCmdHandler getJsonHandler(jsmntok_t** ppTok, char* pJsonReq) {
JSON_TMP_BUFFSIZE	Software/firmware/src/webjson.c	9092	#define JSON_TMP_BUFFSIZE     1024
JSON_HANDLER_PROLOG	Software/firmware/src/webjson.c	9128	#define JSON_HANDLER_PROLOG(cmd) \
JSON_HANDLER_EPILOG	Software/firmware/src/webjson.c	9521	#define JSON_HANDLER_EPILOG() \
JSON_HANDLER	Software/firmware/src/webjson.c	10481	JSON_HANDLER(getInitState) {
JSON_HANDLER	Software/firmware/src/webjson.c	10681	JSON_HANDLER(initPeripherals) {
JSON_HANDLER	Software/firmware/src/webjson.c	10913	JSON_HANDLER(getVersion) {
JSON_HANDLER	Software/firmware/src/webjson.c	11077	JSON_HANDLER(getIpAddr) {
JSON_HANDLER	Software/firmware/src/webjson.c	11337	JSON_HANDLER(getUname) {
JSON_HANDLER	Software/firmware/src/webjson.c	11829	JSON_HANDLER(getSysInfo) {
JSON_HANDLER	Software/firmware/src/webjson.c	13120	JSON_HANDLER(getPhiUptime) {
JSON_HANDLER	Software/firmware/src/webjson.c	14452	JSON_HANDLER(setPower) {
JSON_HANDLER	Software/firmware/src/webjson.c	16532	JSON_HANDLER(setMCtlId) {
JSON_HANDLER	Software/firmware/src/webjson.c	17990	JSON_HANDLER(getGyroDeltas) {
JSON_HANDLER	Software/firmware/src/webjson.c	18441	JSON_HANDLER(getGyroTemp) {
JSON_HANDLER	Software/firmware/src/webjson.c	18793	JSON_HANDLER(getLinkState) {
JSON_HANDLER	Software/firmware/src/webjson.c	19414	JSON_HANDLER(startPhiLink) {
HTTP_BUFFSIZE	Software/firmware/src/webadmin.c	1201	#define HTTP_BUFFSIZE           8096      // max size of HTTP request
FORBIDDEN	Software/firmware/src/webadmin.c	1271	#define FORBIDDEN               403
NOTFOUND	Software/firmware/src/webadmin.c	1307	#define NOTFOUND                404
HTTP_METHOD_HEAD_STR	Software/firmware/src/webadmin.c	1344	#define HTTP_METHOD_HEAD_STR		"HEAD"
HTTP_METHOD_GET_STR	Software/firmware/src/webadmin.c	1381	#define HTTP_METHOD_GET_STR			"GET"
HTTP_METHOD_POST_STR	Software/firmware/src/webadmin.c	1417	#define HTTP_METHOD_POST_STR		"POST"
HTTP_CONT_LEN	Software/firmware/src/webadmin.c	1454	#define HTTP_CONT_LEN						"Content-Length"
HTTP_SERVER_NAME	Software/firmware/src/webadmin.c	1498	#define HTTP_SERVER_NAME        "Phi Web Admin V" PHI_VERSION
pFileExt	Software/firmware/src/webadmin.c	1569		char *pFileExt;
pContentType	Software/firmware/src/webadmin.c	1586		char *pContentType;
wa_extensions	Software/firmware/src/webadmin.c	1602	} wa_extensions [] = {
pMethod	Software/firmware/src/webadmin.c	1974	  char* pMethod;
pPath	Software/firmware/src/webadmin.c	1991	  char* pPath;
pGetParams	Software/firmware/src/webadmin.c	2006	  char* pGetParams;
pHdrContentLength	Software/firmware/src/webadmin.c	2026	  char* pHdrContentLength;
pBody	Software/firmware/src/webadmin.c	2053	  char* pBody;
PHI_PARSED_HTTP	Software/firmware/src/webadmin.c	2062	} PHI_PARSED_HTTP;
wa_response_ok_hdr	Software/firmware/src/webadmin.c	2085	char wa_response_ok_hdr[] =
wa_response_json_hdr	Software/firmware/src/webadmin.c	2282	char wa_response_json_hdr[] =
wa_response_forbidden_hdr	Software/firmware/src/webadmin.c	2495	char wa_response_forbidden_hdr[] =
wa_response_forbidden_body	Software/firmware/src/webadmin.c	2683	char wa_response_forbidden_body[] =
wa_response_notfound_hdr	Software/firmware/src/webadmin.c	2933	char wa_response_notfound_hdr[] =
wa_response_notfound_body	Software/firmware/src/webadmin.c	3120	char wa_response_notfound_body[] =
wa_response_servererror_hdr	Software/firmware/src/webadmin.c	3352	char wa_response_servererror_hdr[] =
wa_response_servererror_body	Software/firmware/src/webadmin.c	3548	char wa_response_servererror_body[] =
SEND_ERROR_REPLY	Software/firmware/src/webadmin.c	4065	#define SEND_ERROR_REPLY(replyType) \
SEND_JSON_REPLY	Software/firmware/src/webadmin.c	4360	#define SEND_JSON_REPLY(pJson) \
phi_webadmin	Software/firmware/src/webadmin.c	4895	void phi_webadmin(int port, const char* wwwRoot)
wa_process_web_request	Software/firmware/src/webadmin.c	7212	void* wa_process_web_request(void* arg)
wa_parseHttpRequest	Software/firmware/src/webadmin.c	11657	int wa_parseHttpRequest(char* pReq, PHI_PARSED_HTTP* pParsed) {
readToken	Software/firmware/src/webadmin.c	13783	char readToken(char** ppData, char** ppToken) {
readToSpace	Software/firmware/src/webadmin.c	15216	void readToSpace(char** ppData, char** ppToken) {
readToEol	Software/firmware/src/webadmin.c	15523	void readToEol(char** ppData, char** ppToken) {
uartFile	Software/firmware/src/uart.c	102	static int uartFile = -1;
uartInit	Software/firmware/src/uart.c	291	BOOL uartInit() {
uart_send	Software/firmware/src/uart.c	2197	void uart_send(BYTE* pData, int dataLen) {
uart_receive	Software/firmware/src/uart.c	2592	int uart_receive(void* pBuff, int buffLen) {
spiFile	Software/firmware/src/spi.c	445	static int spiFile[2] = { -1, -1 };
spi_0_xfer	Software/firmware/src/spi.c	596	struct spi_ioc_transfer spi_0_xfer[2] = {
spi_1_xfer	Software/firmware/src/spi.c	890	struct spi_ioc_transfer spi_1_xfer[2] = {
spi_xfer	Software/firmware/src/spi.c	1152	struct spi_ioc_transfer* spi_xfer[2] = {
spiInit	Software/firmware/src/spi.c	1292	BOOL spiInit() {
spi_send	Software/firmware/src/spi.c	2766	void spi_send(int spiIdx, BYTE* pTx, int txLen) {
spi_receive	Software/firmware/src/spi.c	3063	void spi_receive(int spiIdx, BYTE* pRx, int rxLen) {
spi_sendreceive	Software/firmware/src/spi.c	3366	void spi_sendreceive(int spiIdx, BYTE* pTx, int txLen, BYTE* pRx, int rxLen) {
spi_exchange	Software/firmware/src/spi.c	3827	void spi_exchange(int spiIdx, BYTE* pTx, BYTE* pRx, int dataLen) {
initSpiDriver	Software/firmware/src/spi.c	4175	int initSpiDriver(char* pDriverName, UINT32 speed, BYTE mode, BYTE bpw) {
phi_allocHelper	Software/firmware/src/phi_utils.c	52	void* phi_allocHelper(int size) {
phi_upTime	Software/firmware/src/phi_utils.c	210	UINT64 phi_upTime() {
phi_abortProcess	Software/firmware/src/phi_utils.c	344	void phi_abortProcess(int rc) {
phi_abortWithMsg	Software/firmware/src/phi_utils.c	470	void phi_abortWithMsg(const char* msg) {
__itoa	Software/firmware/src/phi_utils.c	649	char* __itoa(int value, char* result, int base) {
phi_logfp	Software/firmware/src/phi_utils.c	1596	FILE *phi_logfp = NULL;
phi_logInit	Software/firmware/src/phi_utils.c	1619	int phi_logInit(char *filename) {
phi_logClose	Software/firmware/src/phi_utils.c	2551	void phi_logClose(void) {
phi_logTimestamp	Software/firmware/src/phi_utils.c	2657	void phi_logTimestamp() {
phi_logMsg	Software/firmware/src/phi_utils.c	2822	void phi_logMsg(const char* tag, const char *fmt, ...) {
getHostIP	Software/firmware/src/phi_utils.c	3209	UINT32 getHostIP() {
phi_setLED	Software/firmware/src/phi_utils.c	4621	void phi_setLED(PHI_LED_COLOR color) {
phi_rand	Software/firmware/src/phi_utils.c	4947	double phi_rand() {
phi_setRealtimePrio	Software/firmware/src/phi_utils.c	5083	BOOL phi_setRealtimePrio(pthread_t thread) {
setNonBlocking	Software/firmware/src/phi_utils.c	6005	int setNonBlocking(int fd)
RX_BEGIN	Software/firmware/src/philink.c	185	  RX_BEGIN,
RX_RECEIVING	Software/firmware/src/philink.c	197	  RX_RECEIVING,
TX_BEGIN	Software/firmware/src/philink.c	213	  TX_BEGIN,
TX_SENDING	Software/firmware/src/philink.c	225	  TX_SENDING,
COMM_STATE	Software/firmware/src/philink.c	244	} COMM_STATE;
startPhiLink	Software/firmware/src/philink.c	348	BOOL startPhiLink(char* ipAddr, int port) {
phi_link_loop	Software/firmware/src/philink.c	1880	void* phi_link_loop(void* arg)
setLinkState	Software/firmware/src/philink.c	3918	void setLinkState(PHILINK_STATE state) {
motorDefs	Software/firmware/src/motor.c	65	MOTOR_DEF motorDefs[] = {
PHI_setMotorPower	Software/firmware/src/motor.c	446	void PHI_setMotorPower(int motorIdx, BYTE power, BOOL bFwd) {
PHI_getJointPosition	Software/firmware/src/motor.c	952	UINT16 PHI_getJointPosition(int motorIdx) {
PHI_setControllerId	Software/firmware/src/motor.c	1740	void PHI_setControllerId(char oldId, char newId) {
GLOBAL_INIT	Software/firmware/src/main.c	64	#define GLOBAL_INIT
main	Software/firmware/src/main.c	356	void main() {
globalInit	Software/firmware/src/main.c	716	void globalInit() {
jsmn_alloc_token	Software/firmware/src/jsmn.c	121	static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, 
jsmn_fill_token	Software/firmware/src/jsmn.c	475	static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type, 
jsmn_parse_primitive	Software/firmware/src/jsmn.c	737	static jsmnerr_t jsmn_parse_primitive(jsmn_parser *parser, const char *js,
jsmn_parse_string	Software/firmware/src/jsmn.c	1819	static jsmnerr_t jsmn_parse_string(jsmn_parser *parser, const char *js,
jsmn_parse	Software/firmware/src/jsmn.c	2989	jsmnerr_t jsmn_parse(jsmn_parser *parser, const char *js, jsmntok_t *tokens, 
jsmn_init	Software/firmware/src/jsmn.c	5938	void jsmn_init(jsmn_parser *parser) {
LOOPS_PER_SEC	Software/firmware/src/hw_pump.c	325	#define LOOPS_PER_SEC       100
LOOP_PERIOD_USEC	Software/firmware/src/hw_pump.c	357	#define LOOP_PERIOD_USEC    ((INT32)(1e6 / LOOPS_PER_SEC))
cmds	Software/firmware/src/hw_pump.c	504	  PHI_CMD_PACKET cmds;
sensors	Software/firmware/src/hw_pump.c	529	  PHI_STATE_PACKET sensors;
PHI_STATE	Software/firmware/src/hw_pump.c	541	} PHI_STATE;
phiState	Software/firmware/src/hw_pump.c	570	static PHI_STATE phiState;
packetId	Software/firmware/src/hw_pump.c	714	static UINT32 packetId = 0;
startHwPump	Software/firmware/src/hw_pump.c	1578	void startHwPump() {
hwPump_UART_thread	Software/firmware/src/hw_pump.c	2904	void* hwPump_UART_thread(void* arg)
hwPump_SPI_thread	Software/firmware/src/hw_pump.c	3050	void* hwPump_SPI_thread(void* arg)
hwPump_I2C_thread	Software/firmware/src/hw_pump.c	3423	void* hwPump_I2C_thread(void* arg)
initState	Software/firmware/src/hw_pump.c	3604	void initState() {
updateState	Software/firmware/src/hw_pump.c	4198	void updateState() {
phi_getStateSnapshot	Software/firmware/src/hw_pump.c	5165	void phi_getStateSnapshot(PHI_STATE_PACKET *p) {
lock_state	Software/firmware/src/hw_pump.c	5363	void lock_state() {
unlock_state	Software/firmware/src/hw_pump.c	5402	void unlock_state() {
phiHal	Software/firmware/src/hal.c	270	HAL_FUNCS phiHal = {
genericHal	Software/firmware/src/hal.c	708	HAL_FUNCS genericHal = {
HAL_init	Software/firmware/src/hal.c	1145	void HAL_init() {
PHI_initPeripherals	Software/firmware/src/hal.c	1632	char* PHI_initPeripherals() {
GENERIC_initPeripherals	Software/firmware/src/hal.c	2285	char* GENERIC_initPeripherals() {
GENERIC_gyroInit	Software/firmware/src/hal.c	2384	BOOL  GENERIC_gyroInit(BOOL bEnableFifo) {
GYRO_UPDATE_EVERY	Software/firmware/src/hal.c	2447	#define GYRO_UPDATE_EVERY 0.01
GENERIC_gyroGetDeltas	Software/firmware/src/hal.c	2477	void  GENERIC_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {
GENERIC_gyroGetTemp	Software/firmware/src/hal.c	2994	INT8 GENERIC_gyroGetTemp(){
GENERIC_setMotorPower	Software/firmware/src/hal.c	3123	void GENERIC_setMotorPower(int motorIdx, BYTE power, BOOL bFwd) {
GENERIC_getJointPosition	Software/firmware/src/hal.c	3218	UINT16 GENERIC_getJointPosition(int motorIdx) {
GENERIC_setControllerId	Software/firmware/src/hal.c	3393	void GENERIC_setControllerId(char oldId, char newId) {
bUseFifo	Software/firmware/src/gyro.c	137	BOOL bUseFifo = TRUE;
zeroX	Software/firmware/src/gyro.c	188	INT16 zeroX = 0;
zeroY	Software/firmware/src/gyro.c	205	INT16 zeroY = 0;
zeroZ	Software/firmware/src/gyro.c	222	INT16 zeroZ = 0;
threshX	Software/firmware/src/gyro.c	267	INT16 threshX = 0;
threshY	Software/firmware/src/gyro.c	286	INT16 threshY = 0;
threshZ	Software/firmware/src/gyro.c	305	INT16 threshZ = 0;
GYRO_MAX_VALUE	Software/firmware/src/gyro.c	410	#define GYRO_MAX_VALUE          250.0f
GYRO_MAX_VALUE_CMD	Software/firmware/src/gyro.c	449	#define GYRO_MAX_VALUE_CMD      GYRO_CR4_FS_250DPS
GYRO_MAX_VALUE_MULT	Software/firmware/src/gyro.c	500	#define GYRO_MAX_VALUE_MULT     GYRO_250DPS_MULT
GYRO_SAMPLE_RATE	Software/firmware/src/gyro.c	550	#define GYRO_SAMPLE_RATE        100.0f
GYRO_SAMPLE_PERIOD	Software/firmware/src/gyro.c	589	#define GYRO_SAMPLE_PERIOD      (1.0f / GYRO_SAMPLE_RATE)
GYRO_SAMPLE_RATE_CMD	Software/firmware/src/gyro.c	647	#define GYRO_SAMPLE_RATE_CMD    GYRO_CR1_DR_100HZ
PHI_gyroInit	Software/firmware/src/gyro.c	730	BOOL PHI_gyroInit(BOOL bEnableFifo) {
gyroReadStatus	Software/firmware/src/gyro.c	2724	BYTE gyroReadStatus() {
gyroReadFifoSrc	Software/firmware/src/gyro.c	2937	BYTE gyroReadFifoSrc() {
gyroReadRawDps	Software/firmware/src/gyro.c	3150	INT16 gyroReadRawDps(BYTE lowRegAddr) {
gyroReadDps	Software/firmware/src/gyro.c	3380	float gyroReadDps(BYTE lowRegAddr) {
gyroReadFifoSlot_raw	Software/firmware/src/gyro.c	3676	void gyroReadFifoSlot_raw(INT16* pX_dps, INT16* pY_dps, INT16* pZ_dps) {
gyroReadFifoSlot	Software/firmware/src/gyro.c	4090	void gyroReadFifoSlot(float* pPitchDps, float* pYawDps, float* pRollDps) {
PHI_gyroGetDeltas_noFifo	Software/firmware/src/gyro.c	5183	void PHI_gyroGetDeltas_noFifo(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {
PHI_gyroGetDeltas_useFifo	Software/firmware/src/gyro.c	6145	void PHI_gyroGetDeltas_useFifo(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {
PHI_gyroGetDeltas	Software/firmware/src/gyro.c	7112	void PHI_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {
PHI_gyroGetTemp	Software/firmware/src/gyro.c	7351	INT8 PHI_gyroGetTemp() {
gyroCalibrate	Software/firmware/src/gyro.c	7792	void gyroCalibrate() {
