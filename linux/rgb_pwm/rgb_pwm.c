#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/io.h>
#include <linux/mutex.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/kstrtox.h>

/*
 * RGB PWM Driver
 * 
 * Device Tree:
 *   rgb_pwm@ff37f430 {
 *       compatible = "weizenegger,rgb-pwm";
 *       reg = <0xff37f430 0x10>;
 *   };
 *
 * Role:
 *   - Binds to DT node with compatible "weizenegger,rgb-pwm".
 *   - Ioremaps the 4 RGB PWM registers:
 *       RED_OFFSET   = 0x00
 *       GREEN_OFFSET = 0x04
 *       BLUE_OFFSET  = 0x08
 *       PERIOD_OFFSET= 0x0C
 *  - Exposes each register as a sysfs attribute (red/green/blue/period).
 *  - Registers a misc char device rgb_pwm that allows read/write access
 *    to all 4 registers via offsets.
 *
 *
 *
*/


#define RED_OFFSET       0x00
#define GREEN_OFFSET     0x04
#define BLUE_OFFSET      0x08
#define PERIOD_OFFSET    0x0C
#define SPAN             0x10

/* struct rgb_pwm_dev - private rgb_pwm device struct
 *
 * @base_addr:   Kernel virtual base address of the mapped reg block.
 * @red_reg:     address of red reg
 * @green_reg:   address of green reg
 * @blue_reg:    address of blue reg
 * @period_reg:  address of period reg
 * @miscdev:     miscdevice used to create char device
 * @lock:        prevent concurrent access to device
 *
 * struct created for each rgb_pwm device
 */

struct rgb_pwm_dev {
    void __iomem *base_addr;
    void __iomem *red_reg;
    void __iomem *green_reg;
    void __iomem *blue_reg;
    void __iomem *period_reg;
    struct miscdevice miscdev;
    struct mutex lock;
};

/* ------------------------- sysfs: red ------------------------- */

static ssize_t red_show(struct device *dev,
                        struct device_attribute *attr,
                        char *buf)
{
    u32 red;
    struct rgb_pwm_dev *priv = dev_get_drvdata(dev);

    red = ioread32(priv->red_reg);
    return scnprintf(buf, PAGE_SIZE, "%u\n", red);
}

static ssize_t red_store(struct device *dev,
                         struct device_attribute *attr,
                         const char *buf, size_t size)
{
    u32 red;
    int ret;
    struct rgb_pwm_dev *priv = dev_get_drvdata(dev);

    ret = kstrtou32(buf, 0, &red);
    if (ret < 0)
        return ret;

    iowrite32(red, priv->red_reg);
    return size;
}

/* ------------------------- sysfs: green ----------------------- */

static ssize_t green_show(struct device *dev,
                          struct device_attribute *attr,
                          char *buf)
{
    u32 green;
    struct rgb_pwm_dev *priv = dev_get_drvdata(dev);

    green = ioread32(priv->green_reg);
    return scnprintf(buf, PAGE_SIZE, "%u\n", green);
}

static ssize_t green_store(struct device *dev,
                           struct device_attribute *attr,
                           const char *buf, size_t size)
{
    u32 green;
    int ret;
    struct rgb_pwm_dev *priv = dev_get_drvdata(dev);

    ret = kstrtou32(buf, 0, &green);
    if (ret < 0)
        return ret;

    iowrite32(green, priv->green_reg);
    return size;
}

/* ------------------------- sysfs: blue ------------------------ */

static ssize_t blue_show(struct device *dev,
                         struct device_attribute *attr,
                         char *buf)
{
    u32 blue;
    struct rgb_pwm_dev *priv = dev_get_drvdata(dev);

    blue = ioread32(priv->blue_reg);
    return scnprintf(buf, PAGE_SIZE, "%u\n", blue);
}

static ssize_t blue_store(struct device *dev,
                          struct device_attribute *attr,
                          const char *buf, size_t size)
{
    u32 blue;
    int ret;
    struct rgb_pwm_dev *priv = dev_get_drvdata(dev);

    ret = kstrtou32(buf, 0, &blue);
    if (ret < 0)
        return ret;

    iowrite32(blue, priv->blue_reg);
    return size;
}

/* ------------------------- sysfs: period ---------------------- */

static ssize_t period_show(struct device *dev,
                           struct device_attribute *attr,
                           char *buf)
{
    u32 period;
    struct rgb_pwm_dev *priv = dev_get_drvdata(dev);

    period = ioread32(priv->period_reg);
    return scnprintf(buf, PAGE_SIZE, "%u\n", period);
}

static ssize_t period_store(struct device *dev,
                            struct device_attribute *attr,
                            const char *buf, size_t size)
{
    u32 period;
    int ret;
    struct rgb_pwm_dev *priv = dev_get_drvdata(dev);

    ret = kstrtou32(buf, 0, &period);
    if (ret < 0)
        return ret;

    iowrite32(period, priv->period_reg);
    return size;
}

/*
 * Sysfs attributes
*/
static DEVICE_ATTR_RW(red);
static DEVICE_ATTR_RW(green);
static DEVICE_ATTR_RW(blue);
static DEVICE_ATTR_RW(period);

static struct attribute *rgb_pwm_attrs[] = {
    &dev_attr_red.attr,
    &dev_attr_green.attr,
    &dev_attr_blue.attr,
    &dev_attr_period.attr,
    NULL,
};

ATTRIBUTE_GROUPS(rgb_pwm);

/* ----------------- char device: read/write -------------------- */

static ssize_t rgb_pwm_read(struct file *file, char __user *buf,
                            size_t count, loff_t *offset)
{
    size_t ret;
    u32 val;

    struct rgb_pwm_dev *priv = container_of(file->private_data,
                               struct rgb_pwm_dev, miscdev);

    if (*offset < 0) {
        return -EINVAL;
    }
    if (*offset >= SPAN) {
        return 0;
    }
    if ((*offset % 0x04) != 0) {
        pr_warn("rgb_pwm_read: unaligned access\n");
        return -EINVAL;
    }

    val = ioread32(priv->base_addr + *offset);

    ret = copy_to_user(buf, &val, sizeof(val));
    if (ret == sizeof(val)) {
        pr_warn("rgb_pwm_read: nothing copied\n");
        return -EFAULT;
    }

    *offset += sizeof(val);
    return sizeof(val);
}

static ssize_t rgb_pwm_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *offset)
{
    size_t ret;
    u32 val;

    struct rgb_pwm_dev *priv = container_of(file->private_data,
                               struct rgb_pwm_dev, miscdev);

    if (*offset < 0) {
        return -EINVAL;
    }
    if (*offset >= SPAN) {
        return 0;
    }
    if ((*offset % 0x04) != 0) {
        pr_warn("rgb_pwm_write: unaligned access\n");
        return -EINVAL;
    }

    mutex_lock(&priv->lock);

    ret = copy_from_user(&val, buf, sizeof(val));
    if (ret != sizeof(val)) {
        val &= 0x0000FFFF;
        iowrite32(val, priv->base_addr + *offset);

        *offset += sizeof(val);
        ret = sizeof(val);
    } else {
        pr_warn("rgb_pwm_write: nothing copied from user space\n");
        ret = -EFAULT;
    }

    mutex_unlock(&priv->lock);
    return ret;
}

static const struct file_operations rgb_pwm_fops = {
    .owner  = THIS_MODULE,
    .read   = rgb_pwm_read,
    .write  = rgb_pwm_write,
    .llseek = default_llseek,
};

/* ----------------- probe / remove / of_match ------------------ */

static int rgb_pwm_probe(struct platform_device *pdev)
{
    struct rgb_pwm_dev *priv;
    struct resource *res;
    int ret;

    priv = devm_kzalloc(&pdev->dev, sizeof(struct rgb_pwm_dev), GFP_KERNEL);
    if (!priv) {
        pr_err("rgb_pwm: Failed to allocate memory\n");
        return -ENOMEM;
    }

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    /*
     * Request and remap the device's memory region. Requesting the region
     * make sure nobody else can use that memory. The memory is remapped
     * into the kernel's virtual address space because we don't have access
     * to physical memory locations.
   */
    priv->base_addr = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(priv->base_addr)) {
        pr_err("rgb_pwm: Failed to request/remap platform device resource\n");
        return PTR_ERR(priv->base_addr);
    }

    priv->red_reg    = priv->base_addr + RED_OFFSET;
    priv->green_reg  = priv->base_addr + GREEN_OFFSET;
    priv->blue_reg   = priv->base_addr + BLUE_OFFSET;
    priv->period_reg = priv->base_addr + PERIOD_OFFSET;

    /* Initialize: LEDs off, full-scale period */
    iowrite32(0, priv->red_reg);
    iowrite32(0, priv->green_reg);
    iowrite32(0, priv->blue_reg);
    iowrite32(0x0FFF, priv->period_reg);

    priv->miscdev.minor  = MISC_DYNAMIC_MINOR;
    priv->miscdev.name   = "rgb_pwm";
    priv->miscdev.fops   = &rgb_pwm_fops;
    priv->miscdev.parent = &pdev->dev;

    mutex_init(&priv->lock);

    ret = misc_register(&priv->miscdev);
    if (ret) {
        pr_err("rgb_pwm: Failed to register misc device\n");
        return ret;
    }

    platform_set_drvdata(pdev, priv);

    pr_info("rgb_pwm_probe successful\n");
    return 0;
}

static void rgb_pwm_remove(struct platform_device *pdev)
{
    struct rgb_pwm_dev *priv = platform_get_drvdata(pdev);

    misc_deregister(&priv->miscdev);

    pr_info("rgb_pwm_remove\n");
}

/*
 * Compatible string
*/
static const struct of_device_id rgb_pwm_of_match[] = {
    { .compatible = "weizenegger,rgb-pwm", },
    { }
};
MODULE_DEVICE_TABLE(of, rgb_pwm_of_match);

static struct platform_driver rgb_pwm_driver = {
    .probe  = rgb_pwm_probe,
    .remove = rgb_pwm_remove,
    .driver = {
        .owner          = THIS_MODULE,
        .name           = "rgb_pwm",
        .of_match_table = rgb_pwm_of_match,
        .dev_groups     = rgb_pwm_groups,
    },
};

module_platform_driver(rgb_pwm_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Benton Weizenegger");
MODULE_DESCRIPTION("rgb_pwm driver");
MODULE_VERSION("1.0");