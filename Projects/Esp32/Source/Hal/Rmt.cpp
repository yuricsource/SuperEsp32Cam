
#include "HalCommon.h"
#include "Rmt.h"
#include "Dwt.h"

namespace Hal
{

Rmt::Rmt(Gpio *IoPins, Gpio::GpioIndex transmitterPin) : _gpio(IoPins), _transmitterPin(transmitterPin)
{
	rmt_config_t config;
	config.rmt_mode = RMT_MODE_TX;
	config.channel = LED_RMT_TX_CHANNEL;
	config.gpio_num = static_cast<gpio_num_t>(_transmitterPin);
	config.mem_block_num = 3;
	config.tx_config.loop_en = false;
	config.tx_config.carrier_en = false;
	config.tx_config.idle_output_en = true;
	config.tx_config.idle_level = static_cast<rmt_idle_level_t>(0);
	config.clk_div = 2;

	ESP_ERROR_CHECK(rmt_config(&config));
	ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
}

Rmt::~Rmt()
{
}

void Rmt::Write(led_state new_state)
{
	setup_rmt_data_buffer(new_state);
	ESP_ERROR_CHECK(rmt_write_items(LED_RMT_TX_CHANNEL, led_data_buffer, LED_BUFFER_ITEMS, false));
	ESP_ERROR_CHECK(rmt_wait_tx_done(LED_RMT_TX_CHANNEL, portMAX_DELAY));
}

void Rmt::setup_rmt_data_buffer(led_state new_state)
{
	for (uint32_t led = 0; led < NUM_LEDS; led++)
	{
		uint32_t bits_to_send = new_state.leds[led];
		uint32_t mask = 1 << (BITS_PER_LED_CMD - 1);
		for (uint32_t bit = 0; bit < BITS_PER_LED_CMD; bit++)
		{
			uint32_t bit_is_set = bits_to_send & mask;

			if (bit_is_set)
				led_data_buffer[led * BITS_PER_LED_CMD + bit] = tOn;
			else
				led_data_buffer[led * BITS_PER_LED_CMD + bit] = tOff;

			mask >>= 1;
		}
	}
}

} // namespace Hal
