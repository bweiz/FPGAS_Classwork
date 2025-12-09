library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity led_patterns_avalon is
    port (
        clk : in    std_logic;
        rst : in    std_logic;

        -- avalon memory mapped slave interface
        avs_read    : in    std_logic;
        avs_write   : in    std_logic;
        avs_address : in    std_logic_vector(1 downto 0);
        avs_readdata : out   std_logic_vector(31 downto 0);
        avs_writedata : in  std_logic_vector(31 downto 0);

        -- External I/O; export to top-level
        push_button : in    std_logic
    );
end entity led_patterns_avalon;

architecture led_patterns_avalon_arch of led_patterns_avalon is

    signal button_status : std_logic;

    signal debounce_timer : unsigned(31 downto 0);

    signal debounced_button : std_logic;
    
begin

    avalon_register_read : process (clk)
    begin
        if rising_edge(clk) and avs_read = '1' then
            case avs_address is
                when "00" => 
                    avs_readdata <= (0 => button_status, others => '0');
            end case;
        end if;
    end process;


    avalon_register_write : process (clk, rst)
    begin
        if rst = '1' then
            button_status <= '0';
        elsif debounced_button = '1' then
            button_status <= '1';
        elsif rising_edge(clk) and avs_write = '1' then 
            case avs_address is
                when "00" =>
                    button_status <= avs_writedata(1);
            end case;
        end if;
    end process;


    debounce : process (clk, rst)
    begin
        if rst = '1' then
            debounced_button <= '0';
            debounce_timer <= (others => '0');
        elsif falling_edge(clk) then
            if debounce_timer > 0 then
                debounced_button <= '0';
                if debounce_timer < 25000000 then
                    debounce_timer <= debounce_timer + 1;
                else
                    debounce_timer <= (others => '0');
                end if;
            elsif push_button = '1' then
                debounced_button <= '1';
                debounce_timer <= (0 => '1', others => '0');
            else
                debounced_button <= '0';
            end if;
        end if;
    end process;

end architecture led_patterns_avalon_arch;
