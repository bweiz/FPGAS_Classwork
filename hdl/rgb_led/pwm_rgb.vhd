library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity pwm_rgb is
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
end entity pwm_rgb;

architecture rtl of pwm_rgb is

    component pwm_controller is
        port (
            clk      : in  std_logic;
            rst      : in  std_logic;
            period     : in  unsigned(10 downto 0);
            duty_cycle   : in  unsigned(17 downto 0);
            output  : out std_logic
        );
    end component pwm_controller;
    
begin
    
    pwm_red_inst : pwm_controller
        port map (
            clk     => clk,
            rst     => rst,
            period  => period,
				duty_cycle    => duty_r,
            output => pwm_r
        );

    pwm_green_inst : pwm_controller
        port map (
            clk     => clk,
            rst     => rst,
				period  => period,
            duty_cycle    => duty_g,
            output => pwm_g
        );

    pwm_blue_inst : pwm_controller
        port map (
            clk     => clk,
            rst     => rst,
				period  => period,
            duty_cycle    => duty_b,
            output => pwm_b
        );

        

end architecture rtl;