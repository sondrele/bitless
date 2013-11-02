----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    11:59:59 10/15/2013 
-- Design Name: 
-- Module Name:    alu_cfpu - Behavioural 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

--input values are two complex numbers (a+ib), (c+id).
entity fpu is
	port (
		a, b, c, d			: in	std_logic_vector(15 downto 0);
		result, result_2	: out	std_logic_vector(15 downto 0);
		aluop_in			: in	alu_operation;
		flags				: out	std_logic_vector(3 downto 0);
		cpu_clk, alu_clk 	: in	std_logic
	);
end fpu;

architecture behaviour of fpu is
	component fp_multiply
		port(
			a				: in std_logic_vector(15 downto 0);
			b				: in std_logic_vector(15 downto 0);
			operation_nd	: in std_logic;
			operation_rfd	: out std_logic;
			clk				: in std_logic;
			result			: out std_logic_vector(15 downto 0);
			underflow		: out std_logic;
			overflow		: out std_logic;
			rdy				: out std_logic
		);
	end component;
	component fp_addsub
		port (
			a			: in	std_logic_vector(15 downto 0);
			b			: in	std_logic_vector(15 downto 0);
			operation	: in	std_logic_vector(5 downto 0);
			result		: out	std_logic_vector(15 downto 0);
			underflow	: out	std_logic;
			overflow	: out	std_logic
		);
	end component;
	
	signal mul_in_a, mul_in_b, mul_result, addsub_in_a, addsub_in_b, addsub_result : std_logic_vector(15 downto 0);
	signal addsub_op : std_logic_vector(5 downto 0);
begin
	multiply: fp_multiply
		port map(
			a				=> mul_in_a,
			b				=> mul_in_b,
			result			=> mul_result,
			underflow		=> flags(3),
			overflow		=> flags(2),
			operation_nd	=> '-',
			operation_rfd	=> '-',
			rdy				=> '-',
			clk				=> cpu_clk
		);
	addsub: fp_addsub
		port map(
			a			=> addsub_in_a,
			b			=> addsub_in_b,
			operation	=> addsub_op,
			result		=> addsub_result,
			underflow	=> flags(1),
			overflow	=> flags(0)
		);
	
	work: process(operation, clk)
	begin
		case op is
			when fp_mul =>
				a		<=	mul_in_a;
				b		<=	mul_in_b;
				result	<=	mul_result;
				
			when fp_add =>
				a		<=	addsub_in_a;
				b		<=	addsub_in_b;
				result	<=	addsub_result;
				addsub_op <= "000000";
				
			when fp_sub =>
				a		<=	addsub_in_a;
				b		<=	addsub_in_b;
				result	<=	addsub_result;
				addsub_op <= "000001";
				
			when fp_mac =>
				mul_in_a	<=	b;
				mul_in_b	<=	b;
				
				addsub_in_a	<=	mul_result;
				addsub_in_b	<=	c;
				
				addsub_op <= "000000";
				result	<=	addsub_result;
				
			when fp_mas =>
				mul_in_a	<=	b;
				mul_in_b	<=	b;
				
				addsub_in_a	<=	mul_result;
				addsub_in_b	<=	c;
				
				addsub_op <= "000001";
				result	<=	addsub_result;
				
		end case;
	end process;
end behaviour;