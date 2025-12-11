library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- PWM Controller
-- Implements a PWM signal based on input period and duty cycle
-- period: 11.5 fixed point in ms (0 to 2047.96875 ms)
--     sets total PWM period in clock ticks
-- duty_cycle: 18.17 fixed point (0 to 1)
--     fraction of full duty cycle
-- output is high for 'high_cycles' clock ticks per period
--
-- How fpga fabric turns register writes into LED brightness
-- How period and duty interact to control LED intensity


entity pwm_controller is
    generic (
        CLK_PERIOD : time := 20 ns
    );
    port (
        clk        : in std_logic;
        rst        : in std_logic;
        period     : unsigned(10 downto 0);
        duty_cycle : unsigned(17 downto 0);
        output     : out std_logic
    );
end entity pwm_controller;

architecture pwm_arch of pwm_controller is

    -- Fixed Point Formats
    constant F_PERIOD           : integer := 5;     -- period : 11.5
    constant F_DUTY             : integer := 17;    -- duty cycle : 18.17

    constant PERIOD_SCALE       : integer := 32;
    constant DUTY_SCALE         : integer := 131072;

    constant CYCLES_PER_MS      : integer := integer(1 ms / CLK_PERIOD);
    constant WIDTH 		        : integer := 20;
    signal count                : unsigned(WIDTH - 1 downto 0) := (others => '0');
    signal period_cycles        : unsigned(WIDTH - 1 downto 0) := (others => '0');
    signal high_cycles          : unsigned(WIDTH - 1 downto 0) := (others => '0');
    signal pwm_reg              : std_logic := '0';

begin

    -- Main PWM Process
    --  - Computes how many clock cycles output should stay high
    --    based on duty_cycle
    --  - Increments counter from 0 to (period-1) and compares 
    --    to high_cycles to set output

    process(clk, rst)

        variable period_int     : integer;
        variable duty_int       : integer;
        variable cycles_v       : integer;
        variable cycles_u       : unsigned(WIDTH - 1 downto 0);
        variable product        : unsigned(WIDTH + duty_cycle'length - 1 downto 0);
        variable high_u         : unsigned(WIDTH - 1 downto 0);
    begin
        if rst = '1' then
            count               <= (others => '0');
            period_cycles       <= (others => '0');
            high_cycles         <= (others => '0');
            pwm_reg             <= '0';

        elsif rising_edge(clk) then
            
            -- Fixed point input to integers
            period_int := to_integer(period);
            duty_int := to_integer(duty_cycle);

            -- Clamp duty to 0,1
            if duty_int > DUTY_SCALE then
                duty_int := DUTY_SCALE;
	        elsif duty_int < 0 then
		        duty_int := 0;
            end if;

            -- Convert period to clock cycles
            cycles_v := (period_int * CYCLES_PER_MS) / PERIOD_SCALE;

            if cycles_v < 1 then
                cycles_v := 1;
            elsif cycles_v > (2 ** WIDTH - 1) then
                cycles_v := 2 ** WIDTH - 1;
            end if;

            cycles_u := to_unsigned(cycles_v, cycles_u'length);

            -- On time in clock cycles
            product := cycles_u * duty_cycle;
            high_u  := product(product'high - 1 downto F_DUTY);

            period_cycles <= cycles_u;
            high_cycles <= high_u;

            -- count and output logic
            if count >= (period_cycles - 1) then
                count <= (others => '0');
            else
                count <= count + 1;
            end if;

            if count < high_cycles then
                pwm_reg <= '1';
            else
                pwm_reg <= '0';
            end if;
            
        end if;
    end process;

    output <= pwm_reg;

end architecture pwm_arch;