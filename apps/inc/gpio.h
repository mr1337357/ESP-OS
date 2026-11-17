int ioctl(int fd, int op, ...); 

#define IOCPARM_MASK    0x7fUL          /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000UL    /* no parameters */
#define IOC_OUT         0x40000000UL    /* copy out parameters */
#define IOC_IN          0x80000000UL    /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
                                        /* 0x20000000 distinguishes new &
                                           old ioctl's */
#define _IO(x,y)        ((long)(IOC_VOID|((x)<<8)|(y)))
#define _IOR(x,y,t)     ((long)(IOC_OUT|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y)))
#define _IOW(x,y,t)     ((long)(IOC_IN|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y)))

#define GPIO_IOCTL 'g'

#define GPIO_IOCTL_SET_MODE _IOW(GPIO_IOCTL, 1, unsigned int)
#define GPIO_IOCTL_GET_MODE _IOR(GPIO_IOCTL, 2, unsigned int)
#define GPIO_IOCTL_SET_PUPD _IOW(GPIO_IOCTL, 3, unsigned int)
#define GPIO_IOCTL_GET_PUPD _IOR(GPIO_IOCTL, 4, unsigned int)

/** @cond */
#define GPIO_MODE_DEF_DISABLE         (0)
#define GPIO_MODE_DEF_INPUT           (1<<0)    ///< bit mask for input
#define GPIO_MODE_DEF_OUTPUT          (1<<1)    ///< bit mask for output
#define GPIO_MODE_DEF_OD              (1<<2)    ///< bit mask for OD mode
/** @endcond */

typedef enum {
    GPIO_MODE_DISABLE = GPIO_MODE_DEF_DISABLE,                                                         /*!< GPIO mode : disable input and output             */
    GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT,                                                             /*!< GPIO mode : input only                           */
    GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT,                                                           /*!< GPIO mode : output only mode                     */
    GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)),                               /*!< GPIO mode : output only with open-drain mode     */
    GPIO_MODE_INPUT_OUTPUT_OD = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)), /*!< GPIO mode : output and input with open-drain mode*/
    GPIO_MODE_INPUT_OUTPUT = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT)),                         /*!< GPIO mode : output and input mode                */
} gpio_mode_t;

typedef enum {
    GPIO_PULLUP_ONLY,               /*!< Pad pull up            */
    GPIO_PULLDOWN_ONLY,             /*!< Pad pull down          */
    GPIO_PULLUP_PULLDOWN,           /*!< Pad pull up + pull down*/
    GPIO_FLOATING,                  /*!< Pad floating           */
} gpio_pull_mode_t;

int gpio_open(int pin);

int gpio_set_mode(int fd, gpio_mode_t mode);

int gpio_set_pupd(int fd, gpio_pull_mode_t pupd);

int gpio_setOutput(int fd, int value);

int gpio_getInput(int fd);

void gpio_close(int fd);