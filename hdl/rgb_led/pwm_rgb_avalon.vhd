library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library std;
use std.standard;

entity pwm_rgb_avalon is
    port (
        clk           : in  std_logic;
        rst           : in  std_logic;

        --Avalon Slave Interface
        avs_read      : in  std_logic;
        avs_write     : in  std_logic;
        avs_address   : in  std_logic_vector(1 downto 0);
        avs_writedata : in  std_logic_vector(31 downto 0);
        avs_readdata  : out std_logic_vector(31 downto 0);
        -- External I/O
        pwm_r         : out std_logic;
        pwm_g         : out std_logic;
        pwm_b         : out std_logic
    );
end entity pwm_rgb_avalon;

architecture rtl of pwm_rgb_avalon is

    signal reg_duty_r   : std_logic_vector(17 downto 0) := (others => '0');
    signal reg_duty_g   : std_logic_vector(17 downto 0) := (others => '0');
    signal reg_duty_b   : std_logic_vector(17 downto 0) := (others => '0');
    signal reg_period   : std_logic_vector(10 downto 0) := (others => '0');

    component pwm_rgb is
        port (
            clk       : in  std_logic;
            rst       : in  std_logic;
            duty_r    : in  unsigned(17 downto 0);
            duty_g    : in  unsigned(17 downto 0);
            duty_b    : in  unsigned(17 downto 0);
            period    : in  unsigned(10 downto 0);
            pwm_r     : out std_logic;
            pwm_g     : out std_logic;
            pwm_b     : out std_logic
        );
    end component pwm_rgb;

begin

    pwm_rgb_inst : pwm_rgb
        port map (
            clk     => clk,
            rst     => rst,
            duty_r  => unsigned(reg_duty_r),
            duty_g  => unsigned(reg_duty_g),
            duty_b  => unsigned(reg_duty_b),
            period  => unsigned(reg_period),
            pwm_r   => pwm_r,
            pwm_g   => pwm_g,
            pwm_b   => pwm_b
        );
    
    avalon_register_read : process(clk)
    begin
        if rising_edge(clk) and avs_read = '1' then
                case avs_address is
                    when "00" =>
                        avs_readdata <= (31 downto 18 => '0') & reg_duty_r;
                    when "01" =>
                        avs_readdata <= (31 downto 18 => '0') & reg_duty_g;
                    when "10" =>
                        avs_readdata <= (31 downto 18 => '0') & reg_duty_b;
                    when "11" =>
                        avs_readdata <= (31 downto 11 => '0') & reg_period;
                    when others =>
                        avs_readdata <= (others => '0');
                end case;
            
        end if;
    end process avalon_register_read;

    avalon_register_write : process(clk, rst)
    begin
        if rst = '1' then
            reg_duty_r <= (others => '0');
            reg_duty_g <= (others => '0');
            reg_duty_b <= (others => '0');
            reg_period <= (others => '0');
        elsif rising_edge(clk) and avs_write = '1' then
            case avs_address is
                when "00" =>
                    reg_duty_r <= avs_writedata(17 downto 0);
                when "01" =>
                    reg_duty_g <= avs_writedata(17 downto 0);
                when "10" =>
                    reg_duty_b <= avs_writedata(17 downto 0);
                when "11" =>
                    reg_period <= avs_writedata(10 downto 0);
                when others =>
                    null;
            end case;
        end if;
    end process avalon_register_write;
end architecture rtl;