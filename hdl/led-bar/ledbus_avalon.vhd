library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library std;
use std.standard;

-- Avalon-MM base address (HPS lightweight bridge):
--   Base: 0x0017F450
--   Span: 0x10 bytes (0x0017F450 – 0x0017F45F)
-- 32-bit register map (word offsets from base):
--   0x0 @ 0x0017F450 : LED pattern register
--       - avs_address = "00"
--       - Bits 9 downto 0 drive the 10 LEDs directly.
--       - Bits 31 downto 10 are ignored on write and read back as '0'.

entity ledbus_avalon is
    port (
        clk             : in  std_logic;
        rst             : in  std_logic;
        avs_read        : in  std_logic;
        avs_write       : in  std_logic;
        avs_address     : in  std_logic_vector(1 downto 0);
        avs_writedata   : in  std_logic_vector(31 downto 0);
        avs_readdata    : out std_logic_vector(31 downto 0);
        led_out         : out std_logic_vector(9 downto 0)
    );
end entity ledbus_avalon;

architecture rtl of ledbus_avalon is

    signal reg_leds   : std_logic_vector(9 downto 0) := (others => '0');
    signal reg_readdata  : std_logic_vector(31 downto 0) := (others => '0');

begin

    led_out <= reg_leds;
    avs_readdata <= reg_readdata;

    avalon_register_read : process(clk)
    begin
        if rising_edge(clk) and avs_read = '1' then
            case avs_address is
                when "00" =>
                    reg_readdata <= (others => '0');
                    reg_readdata(9 downto 0) <= reg_leds;
                when others =>
                    reg_readdata <= (others => '0');
            end case;
        end if;
    end process avalon_register_read;

    avalon_register_write : process(clk)
    begin
        if rst = '1' then
            reg_leds <= (others => '0');
        elsif rising_edge(clk) and avs_write = '1' then
            case avs_address is
                when "00" =>
                    reg_leds <= avs_writedata(9 downto 0);
                when others =>
                    null;
            end case;
        end if;
    end process avalon_register_write;
end architecture rtl;