#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/io.h>
#include <linux/mutex.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/kstrtox.h>

#define SPAN 16


struct push_button_dev {
    void __iomem *base_addr;
    void __iomem *button_reg;
    struct miscdevice miscdev;
    struct mutex lock;
};


static ssize_t push_button_reg_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
    u8 button_reg;
    struct push_button_dev *priv = dev_get_drvdata(dev);
    button_reg = ioread32(priv->button_reg);
    return scnprintf(buf, PAGE_SIZE, "%u\n", button_reg);
}


static ssize_t push_button_reg_store(struct device *dev,
    struct device_attribute *attr, const char *buf, size_t size)
{
    u8 button_reg;
    int ret;
    struct push_button_dev *priv = dev_get_drvdata(dev);
    // Parse the string we received as a u8
    // See https://elixir.bootlin.com/linux/latest/source/lib/kstrtox.c#L289
    ret = kstrtou8(buf, 0, &button_reg);
    if (ret < 0) {
        return ret;
    }
    iowrite32(button_reg, priv->button_reg);
    // Write was successful, so we return the number of bytes we wrote.
    return size;
}

// Define sysfs attributes
static DEVICE_ATTR_RW(push_button_reg);
// Create an attribute group so the device core can
// export the attributes for us.
static struct attribute *push_button_attrs[] = {
    &dev_attr_push_button_reg.attr,
    NULL,
};
ATTRIBUTE_GROUPS(push_button);

static ssize_t push_button_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    size_t ret;
    u32 val;

    struct push_button_dev *priv = container_of(file->private_data, struct push_button_dev, miscdev);

    if (*offset < 0) {
        return -EINVAL;
    }
    if (*offset >= SPAN) {
        return 0;
    }
    if ((*offset % 0x4) != 0) {
        pr_warn("push_button_read: unaligned access\n");
        return -EFAULT;
    }

    val = ioread32(priv->base_addr + *offset);

    ret = copy_to_user(buf, &val, sizeof(val));
    if (ret == sizeof(val)) {
        pr_warn("push_button_read: nothing copied\n");
        return -EFAULT;
    }


    *offset = *offset + sizeof(val);

    return sizeof(val);
}

static ssize_t push_button_write(struct file *file, const char __user *buf,
    size_t count, loff_t *offset)
{
    size_t ret;
    u32 val;

    struct push_button_dev *priv = container_of(file->private_data,
        struct push_button_dev, miscdev);

    if (*offset < 0) {
        return -EINVAL;
    }
    if (*offset >= SPAN) {
        return 0;
    }
    if ((*offset % 0x4) != 0) {
        pr_warn("push_button_write: unaligned access\n");
        return -EFAULT;
    }

    mutex_lock(&priv->lock);
    // Get the value from userspace.
    ret = copy_from_user(&val, buf, sizeof(val));
    if (ret != sizeof(val)) {
        iowrite32(val, priv->base_addr + *offset);
        // Increment the file offset by the number of bytes we wrote.
        *offset = *offset + sizeof(val);
        // Return the number of bytes we wrote.
        ret = sizeof(val);
    }
    else {
        pr_warn("push_button_write: nothing copied from user space\n");
        ret = -EFAULT;
    }

    mutex_unlock(&priv->lock);
    return ret;
}



/**
* led_patterns_fops - File operations supported by the
* led_patterns driver
* @owner: The led_patterns driver owns the file operations; this
* ensures that the driver can't be removed while the
* character device is still in use.
* @read: The read function.
* @write: The write function.
* @llseek: We use the kernel's default_llseek() function; this allows
* users to change what position they are writing/reading to/from.
*/
static const struct file_operations push_button_fops = {
    .owner = THIS_MODULE,
    .read = push_button_read,
    .write = push_button_write,
    .llseek = default_llseek,
};


/**
* led_patterns_probe() - Initialize device when a match is found
* @pdev: Platform device structure associated with our led patterns device;
* pdev is automatically created by the driver core based upon our
* led patterns device tree node.
*
* When a device that is compatible with this led patterns driver is found, the
* driver's probe function is called. This probe function gets called by the
* kernel when an led_patterns device is found in the device tree.
*/
static int push_button_probe(struct platform_device *pdev)
{
   struct push_button_dev *priv;
   size_t ret;

    /*
    * Allocate kernel memory for the led patterns device and set it to 0.
    * GFP_KERNEL specifies that we are allocating normal kernel RAM;
    * see the kmalloc documentation for more info. The allocated memory
    * is automatically freed when the device is removed.
    */
    priv = devm_kzalloc(&pdev->dev, sizeof(*priv),
    GFP_KERNEL);
    if (!priv) {
        pr_err("Push Button:Failed to allocate memory\n");
        return -ENOMEM;
    }

    /*
    * Request and remap the device's memory region. Requesting the region
    * make sure nobody else can use that memory. The memory is remapped
    * into the kernel's virtual address space because we don't have access
    * to physical memory locations.
    */
    priv->base_addr = devm_platform_ioremap_resource(pdev, 0);
    if (IS_ERR(priv->base_addr)) {
        pr_err("Failed to request/remap platform device resource\n");
        return PTR_ERR(priv->base_addr);
    }

    // Set the memory addresses for each register.
    priv->button_reg = priv->base_addr;


    // Initialize the misc device parameters
    priv->miscdev.minor = MISC_DYNAMIC_MINOR;
    priv->miscdev.name = "push_button";
    priv->miscdev.fops = &push_button_fops;
    priv->miscdev.parent = &pdev->dev;
    // Register the misc device; this creates a char dev at /dev/push_button
    ret = misc_register(&priv->miscdev);
    if (ret) {
        pr_err("Failed to register misc device");
        return ret;
    }
    /*
    * Attach the led patterns's private data to the platform device's struct.
    * This is so we can access our state container in the other functions.
    */
    platform_set_drvdata(pdev, priv);

    pr_info("push button probe successful\n");

    return 0;
}

/**
* led_patterns_remove() - Remove an led patterns device.
* @pdev: Platform device structure associated with our led patterns device.
*
* This function is called when an led patterns devicee is removed or
* the driver is removed.
*/
static void push_button_remove(struct platform_device *pdev)
{
    // Get the led patterns's private data from the platform device.
    struct push_button_dev *priv = platform_get_drvdata(pdev);
    // Disable software-control mode, just for kicks.

    // Deregister the misc device and remove the /dev/led_patterns file.
    misc_deregister(&priv->miscdev);

    pr_info("push button remove successful\n");
}


/*
* Define the compatible property used for matching devices to this driver,
* then add our device id structure to the kernel's device table. For a device
* to be matched with this driver, its device tree node must use the same
* compatible string as defined here.
*/
static const struct of_device_id push_button_of_match[] = {
    { .compatible = "sdc,push_button", },
    { }
};
MODULE_DEVICE_TABLE(of, push_button_of_match);


/**
* struct led_patterns_driver - Platform driver struct for the led_patterns driver
* @probe: Function that's called when a device is found
* @remove: Function that's called when a device is removed
* @driver.owner: Which module owns this driver
* @driver.name: Name of the led_patterns driver
* @driver.of_match_table: Device tree match table
*/
static struct platform_driver push_button_driver = {
    .probe = push_button_probe,
    .remove = push_button_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "push_button",
        .of_match_table = push_button_of_match,
        .dev_groups = push_button_groups,
    },
};
/*
* We don't need to do anything special in module init/exit.
* This macro automatically handles module init/exit.
*/
module_platform_driver(push_button_driver);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("push_button driver");
MODULE_VERSION("1.0");