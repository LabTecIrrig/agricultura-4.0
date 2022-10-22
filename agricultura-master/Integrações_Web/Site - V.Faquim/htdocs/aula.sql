-- phpMyAdmin SQL Dump
-- version 4.5.1
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: 22-Jun-2017 às 19:35
-- Versão do servidor: 10.1.19-MariaDB
-- PHP Version: 5.6.28

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `aula`
--

-- --------------------------------------------------------

--
-- Estrutura da tabela `arduino`
--

CREATE TABLE `arduino` (
  `id` int(11) NOT NULL,
  `inf` varchar(45) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `arduino`
--

INSERT INTO `arduino` (`id`, `inf`) VALUES
(1, 'ads'),
(2, 'kacala'),
(3, 'oi'),
(4, 'oi'),
(5, 'oi'),
(6, 'oi'),
(7, 'oi'),
(8, 'oi'),
(9, 'oi'),
(10, 'oi'),
(11, 'oi'),
(12, 'oi'),
(13, 'oi'),
(14, 'oi'),
(15, 'oi'),
(16, 'oi'),
(17, 'oi'),
(18, 'oi'),
(19, 'oi'),
(20, 'oi'),
(21, 'oi'),
(22, 'oi'),
(23, 'oi'),
(24, 'oi'),
(25, 'oi'),
(26, 'oi'),
(27, 'oi'),
(28, 'oi'),
(29, 'oi'),
(30, 'oi'),
(31, 'oi');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `arduino`
--
ALTER TABLE `arduino`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `arduino`
--
ALTER TABLE `arduino`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=32;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
