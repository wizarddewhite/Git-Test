-- MySQL dump 10.13  Distrib 5.1.66, for debian-linux-gnu (i686)
--
-- Host: localhost    Database: STUCOU
-- ------------------------------------------------------
-- Server version	5.1.66-0ubuntu0.11.10.2

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `COURSE`
--

DROP TABLE IF EXISTS `COURSE`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `COURSE` (
  `CID` int(11) NOT NULL AUTO_INCREMENT,
  `CNAME` varchar(40) COLLATE utf8_unicode_ci NOT NULL,
  `TID` int(11) NOT NULL,
  PRIMARY KEY (`CID`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `COURSE`
--

LOCK TABLES `COURSE` WRITE;
/*!40000 ALTER TABLE `COURSE` DISABLE KEYS */;
INSERT INTO `COURSE` VALUES (1,'Data Structures',1),(2,'Operating System',2),(3,'Database System',3),(4,'Networking',4),(5,'Computer System Architecture',5),(6,'Software Engineering',6),(7,'Circuits and Electronics',7),(8,'The C Programming Language',8),(9,'The C++ Programming Language',9),(10,'Signals and Systems',10),(11,'Introduction to Algorithms',1),(12,'Distributed Computer Systems',2);
/*!40000 ALTER TABLE `COURSE` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `STUCOU`
--

DROP TABLE IF EXISTS `STUCOU`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `STUCOU` (
  `SID` int(11) NOT NULL,
  `CID` int(11) NOT NULL,
  `SCORE` int(11) DEFAULT NULL,
  PRIMARY KEY (`SID`,`CID`),
  KEY `CID` (`CID`),
  CONSTRAINT `STUCOU_ibfk_3` FOREIGN KEY (`SID`) REFERENCES `STUDENT` (`SID`),
  CONSTRAINT `STUCOU_ibfk_4` FOREIGN KEY (`CID`) REFERENCES `COURSE` (`CID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `STUCOU`
--

LOCK TABLES `STUCOU` WRITE;
/*!40000 ALTER TABLE `STUCOU` DISABLE KEYS */;
INSERT INTO `STUCOU` VALUES (1,1,54),(1,4,61),(1,6,87),(1,7,84),(2,2,72),(2,4,83),(2,5,67),(2,10,57),(3,2,85),(3,4,71),(3,5,79),(3,6,50),(3,8,85),(3,9,91),(4,1,71),(4,3,62),(4,4,65),(4,6,82),(4,8,84),(5,3,92),(5,12,85),(6,1,71),(6,8,56),(6,9,93),(6,12,92),(7,1,67),(7,3,61),(7,4,72),(7,8,70),(7,9,82),(9,4,70),(9,8,92),(9,12,53),(10,1,64),(10,2,96),(10,3,62),(10,4,79),(10,5,72),(10,8,50),(10,11,78),(11,2,56),(11,4,74),(11,6,57),(11,10,89),(11,12,57),(12,3,53),(12,5,77),(12,11,70),(13,4,97),(13,8,96),(13,9,75),(14,2,100),(14,3,63),(14,5,97),(14,7,50),(14,9,96),(14,10,65),(15,3,70),(15,5,77),(15,6,94),(15,7,100),(15,8,87),(15,9,92),(15,12,53),(16,1,78),(16,3,88),(16,5,89),(16,10,73),(16,12,62),(17,1,70),(17,2,62),(17,7,55),(18,9,63),(18,12,94),(19,4,72),(19,6,77),(20,3,99),(20,4,66),(20,7,77),(21,5,53),(21,6,59),(21,9,74),(21,11,62),(22,4,66),(22,6,60),(23,1,67),(23,4,90),(23,6,62),(23,10,73),(24,8,79),(25,2,99),(25,6,66),(25,8,100),(25,10,58),(26,1,87),(26,6,56),(26,8,97),(26,9,88),(26,10,82),(27,2,80),(27,4,78),(27,5,90),(27,7,65),(27,8,70),(27,9,60),(27,10,53),(27,11,64),(28,6,62),(28,9,79),(28,10,67),(28,11,61),(29,5,83),(29,7,90),(29,8,52),(29,11,89),(29,12,54),(30,3,91),(30,4,74),(30,5,74),(30,8,71),(30,10,91),(30,11,76),(30,12,89),(31,1,54),(31,3,54),(31,5,92),(31,10,76),(31,12,99),(32,3,53),(32,6,64),(32,8,99),(32,9,64),(32,10,70),(32,12,62),(33,3,59),(33,8,90),(33,9,56),(33,10,69),(33,12,91),(34,2,93),(34,3,79),(34,7,99),(34,9,73),(34,11,66),(35,5,61),(35,7,73),(35,10,78),(35,11,92),(36,1,84),(36,4,60),(36,9,82),(36,11,70),(37,2,64),(37,3,54),(37,4,51),(37,7,73),(37,12,50),(38,1,95),(38,2,83),(38,9,66),(38,10,83),(39,8,95),(39,10,86),(40,3,87),(40,4,86),(40,6,63),(41,4,78),(41,5,50),(41,11,55),(42,2,96),(42,3,78),(42,4,64),(42,12,83),(43,2,78),(43,4,50),(43,8,68),(43,9,83),(43,11,73),(44,2,55),(44,3,64),(44,8,92),(45,8,60),(45,10,90),(45,11,99),(45,12,98),(46,6,71),(46,7,80),(46,9,84),(47,6,100),(47,10,81),(48,9,54),(48,10,90),(49,1,79),(49,3,76),(49,5,78),(50,2,80),(50,6,95),(50,9,97),(50,10,78),(50,11,99),(51,2,50),(51,4,62),(51,9,75),(51,11,88),(52,2,97),(52,6,88),(52,9,55),(53,9,82),(54,8,79),(54,9,60),(54,11,100),(56,10,95),(56,12,86),(57,2,61),(57,4,68),(58,1,53),(58,6,70),(58,7,61),(58,8,65),(58,10,80),(58,11,68),(58,12,52),(59,3,68),(59,6,78),(59,7,69),(59,9,79),(59,10,67),(60,3,100),(60,4,85),(60,8,70),(60,10,56),(60,12,83),(61,5,77),(61,7,51),(61,9,52),(61,11,78),(62,3,73),(62,4,93),(62,5,80),(62,11,92),(63,6,65),(64,6,87),(64,8,79),(64,11,60),(65,2,78),(65,3,81),(65,10,94),(65,11,58),(65,12,80),(66,2,67),(66,9,53),(67,7,69),(67,10,99),(67,11,74),(67,12,68),(68,1,91),(68,3,91),(68,4,95),(68,5,79),(68,6,82),(68,7,95),(68,10,77),(69,7,53),(69,11,86);
/*!40000 ALTER TABLE `STUCOU` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `STUDENT`
--

DROP TABLE IF EXISTS `STUDENT`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `STUDENT` (
  `SID` int(11) NOT NULL AUTO_INCREMENT,
  `SNAME` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `PASSWORD` text COLLATE utf8_unicode_ci,
  PRIMARY KEY (`SID`)
) ENGINE=InnoDB AUTO_INCREMENT=70 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `STUDENT`
--

LOCK TABLES `STUDENT` WRITE;
/*!40000 ALTER TABLE `STUDENT` DISABLE KEYS */;
INSERT INTO `STUDENT` VALUES (1,'Hop Whitehead','a7d0d67f1216fe10a28490a8f2e8744a'),(2,'Oscar Stein','a7d0d67f1216fe10a28490a8f2e8744a'),(3,'Hilel Mendoza','a7d0d67f1216fe10a28490a8f2e8744a'),(4,'Fritz Copeland','a7d0d67f1216fe10a28490a8f2e8744a'),(5,'Brody Todd','a7d0d67f1216fe10a28490a8f2e8744a'),(6,'Vaughan Sweet','a7d0d67f1216fe10a28490a8f2e8744a'),(7,'Hoyt Fisher','a7d0d67f1216fe10a28490a8f2e8744a'),(8,'Kenyon Small','a7d0d67f1216fe10a28490a8f2e8744a'),(9,'Denton Crawford','a7d0d67f1216fe10a28490a8f2e8744a'),(10,'Alan Hobbs','a7d0d67f1216fe10a28490a8f2e8744a'),(11,'Xander Bishop','a7d0d67f1216fe10a28490a8f2e8744a'),(12,'Leroy Castro','a7d0d67f1216fe10a28490a8f2e8744a'),(13,'Elvis Lawson','a7d0d67f1216fe10a28490a8f2e8744a'),(14,'Zahir Nicholson','a7d0d67f1216fe10a28490a8f2e8744a'),(15,'Palmer Lee','a7d0d67f1216fe10a28490a8f2e8744a'),(16,'Jasper Dale','a7d0d67f1216fe10a28490a8f2e8744a'),(17,'Ivor Duran','a7d0d67f1216fe10a28490a8f2e8744a'),(18,'Basil Watkins','a7d0d67f1216fe10a28490a8f2e8744a'),(19,'Sylvester Hull','a7d0d67f1216fe10a28490a8f2e8744a'),(20,'Byron Anderson','a7d0d67f1216fe10a28490a8f2e8744a'),(21,'Fitzgerald Bowman','a7d0d67f1216fe10a28490a8f2e8744a'),(22,'Timothy Scott','a7d0d67f1216fe10a28490a8f2e8744a'),(23,'Galvin Kelley','a7d0d67f1216fe10a28490a8f2e8744a'),(24,'Melvin Nash','a7d0d67f1216fe10a28490a8f2e8744a'),(25,'Nash Patrick','a7d0d67f1216fe10a28490a8f2e8744a'),(26,'Barrett Nolan','a7d0d67f1216fe10a28490a8f2e8744a'),(27,'Quamar Patrick','a7d0d67f1216fe10a28490a8f2e8744a'),(28,'Rudyard Bruce','a7d0d67f1216fe10a28490a8f2e8744a'),(29,'Conan Schmidt','a7d0d67f1216fe10a28490a8f2e8744a'),(30,'Preston Woods','a7d0d67f1216fe10a28490a8f2e8744a'),(31,'Felix Fisher','a7d0d67f1216fe10a28490a8f2e8744a'),(32,'Christopher Shields','a7d0d67f1216fe10a28490a8f2e8744a'),(33,'Justin Flowers','a7d0d67f1216fe10a28490a8f2e8744a'),(34,'Rahim Hodge','a7d0d67f1216fe10a28490a8f2e8744a'),(35,'Cleo Osborne','a7d0d67f1216fe10a28490a8f2e8744a'),(36,'Odessa Fulton','a7d0d67f1216fe10a28490a8f2e8744a'),(37,'Shaine Blackburn','a7d0d67f1216fe10a28490a8f2e8744a'),(38,'Blythe Rose','a7d0d67f1216fe10a28490a8f2e8744a'),(39,'Nevada Guthrie','a7d0d67f1216fe10a28490a8f2e8744a'),(40,'Jeanette Fowler','a7d0d67f1216fe10a28490a8f2e8744a'),(41,'Anjolie Terrell','a7d0d67f1216fe10a28490a8f2e8744a'),(42,'Amy Dejesus','a7d0d67f1216fe10a28490a8f2e8744a'),(43,'Emma Hawkins','a7d0d67f1216fe10a28490a8f2e8744a'),(44,'Raya Anthony','a7d0d67f1216fe10a28490a8f2e8744a'),(45,'April Castro','a7d0d67f1216fe10a28490a8f2e8744a'),(46,'Vera Burch','a7d0d67f1216fe10a28490a8f2e8744a'),(47,'Yen Warren','a7d0d67f1216fe10a28490a8f2e8744a'),(48,'Brynn Davenport','a7d0d67f1216fe10a28490a8f2e8744a'),(49,'Lee Wagner','a7d0d67f1216fe10a28490a8f2e8744a'),(50,'Ursa Reid','a7d0d67f1216fe10a28490a8f2e8744a'),(51,'Marny Montoya','a7d0d67f1216fe10a28490a8f2e8744a'),(52,'Audrey Irwin','a7d0d67f1216fe10a28490a8f2e8744a'),(53,'Nora Cobb','a7d0d67f1216fe10a28490a8f2e8744a'),(54,'Elizabeth Hayden','a7d0d67f1216fe10a28490a8f2e8744a'),(55,'Lavinia Padilla','a7d0d67f1216fe10a28490a8f2e8744a'),(56,'Noelle Medina','a7d0d67f1216fe10a28490a8f2e8744a'),(57,'Macy Raymond','a7d0d67f1216fe10a28490a8f2e8744a'),(58,'Shana Spears','a7d0d67f1216fe10a28490a8f2e8744a'),(59,'Carol Mendoza','a7d0d67f1216fe10a28490a8f2e8744a'),(60,'Summer Curtis','a7d0d67f1216fe10a28490a8f2e8744a'),(61,'Reagan Sanders','a7d0d67f1216fe10a28490a8f2e8744a'),(62,'Halee Ramsey','a7d0d67f1216fe10a28490a8f2e8744a'),(63,'Quinn Hunter','a7d0d67f1216fe10a28490a8f2e8744a'),(64,'Yen Fry','a7d0d67f1216fe10a28490a8f2e8744a'),(65,'Indigo Tanner','a7d0d67f1216fe10a28490a8f2e8744a'),(66,'Astra Arnold','a7d0d67f1216fe10a28490a8f2e8744a'),(67,'Kylan Carter','a7d0d67f1216fe10a28490a8f2e8744a'),(68,'Katell Delgado','a7d0d67f1216fe10a28490a8f2e8744a'),(69,'Halee Ramsey','a7d0d67f1216fe10a28490a8f2e8744a');
/*!40000 ALTER TABLE `STUDENT` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `TEACHER`
--

DROP TABLE IF EXISTS `TEACHER`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `TEACHER` (
  `TID` int(11) NOT NULL AUTO_INCREMENT,
  `TNAME` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `PASSWORD` text COLLATE utf8_unicode_ci,
  PRIMARY KEY (`TID`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `TEACHER`
--

LOCK TABLES `TEACHER` WRITE;
/*!40000 ALTER TABLE `TEACHER` DISABLE KEYS */;
INSERT INTO `TEACHER` VALUES (1,'Nichole Becker','a7d0d67f1216fe10a28490a8f2e8744a'),(2,'Rana Bush','a7d0d67f1216fe10a28490a8f2e8744a'),(3,'Sylvia Blackwell','a7d0d67f1216fe10a28490a8f2e8744a'),(4,'Odessa Walton','a7d0d67f1216fe10a28490a8f2e8744a'),(5,'Piper Durham','a7d0d67f1216fe10a28490a8f2e8744a'),(6,'Nash Dixon','a7d0d67f1216fe10a28490a8f2e8744a'),(7,'Hu Pratt','a7d0d67f1216fe10a28490a8f2e8744a'),(8,'Fuller Levine','a7d0d67f1216fe10a28490a8f2e8744a'),(9,'Simon Bryant','a7d0d67f1216fe10a28490a8f2e8744a'),(10,'Merritt Sampson','a7d0d67f1216fe10a28490a8f2e8744a');
/*!40000 ALTER TABLE `TEACHER` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2012-11-29 15:28:14
