#include <zephyr/kernel.h>
#include <app_version.h>
#include <zephyr/drivers/w1.h>
#include <zephyr/devicetree.h>
#include <zephyr/shell/shell.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

static const struct device *const w1_dev = DEVICE_DT_GET(DT_NODELABEL(w1));

int main(void)
{

	if (!device_is_ready(w1_dev)) {
		LOG_ERR("Device not ready");
		return 0;
	}

	return 0;
}

static void w1_search_callback(struct w1_rom rom, void *user_data)
{
	const struct shell *sh = (const struct shell *)user_data;

	shell_info(sh, "Device found; family: 0x%02x, serial: 0x%016llx", rom.family,
		w1_rom_to_uint64(&rom));
}

static int cmd_w1_search(const struct shell *sh, size_t argc, char **argv)
{
	int num_devices = w1_search_rom(w1_dev, w1_search_callback, (void*)sh);

	shell_info(sh, "Number of devices found on bus: %d", num_devices);

	return 0;
}

static uint64_t last_read;

static int cmd_w1_read(const struct shell *sh, size_t argc, char **argv)
{
	struct w1_rom rom = {0};

	int ret = w1_read_rom(w1_dev, &rom);
	if (ret < 0) {
		shell_error(sh, "Error reading ROM: %d", ret);
		return 0;
	}

	last_read = w1_rom_to_uint64(&rom);
	shell_info(sh, "Serial: 0x%016llx", last_read);

	return 0;
}

static uint64_t stored;

static int cmd_w1_store(const struct shell *sh, size_t argc, char **argv)
{
	stored = last_read;

	shell_info(sh, "Stored serial 0x%016llx", stored);

	return 0;
}

static int cmd_w1_write(const struct shell *sh, size_t argc, char **argv)
{
	struct w1_slave_config slave;
	struct w1_rom to_write, result;
	int ret;

	if (stored == 0) {
		shell_error(sh, "No serial stored");
		return -1;
	}

	w1_uint64_to_rom(stored, &to_write);

	ret = w1_read_rom(w1_dev, &slave.rom);
	if (ret < 0) {
		shell_error(sh, "No device found");
		return -1;
	}

	ret = w1_write_read(w1_dev, &slave, (uint8_t*)&to_write, sizeof(to_write), (uint8_t*)&result, sizeof(result));
	if (ret < 0) {
		shell_error(sh, "Writing failed");
		return -1;
	}

	shell_info(sh, "Read back serial: 0x%016llx", w1_rom_to_uint64(&result));

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(w1,
	SHELL_CMD(search, NULL, "Search 1Wire devices", cmd_w1_search),
	SHELL_CMD(read, NULL, "Read single 1Wire device", cmd_w1_read),
	SHELL_CMD(store, NULL, "Store last read serial for writing", cmd_w1_store),
	SHELL_CMD(write, NULL, "Write stored serial", cmd_w1_write),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(w1, &w1, "1Wire commands", NULL);


