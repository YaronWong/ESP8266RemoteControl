/*
 Navicat Premium Data Transfer

 Source Server         : tencent-70-mcu
 Source Server Type    : MySQL
 Source Server Version : 50650
 Source Host           : 101.35.199.224:3306
 Source Schema         : mcu

 Target Server Type    : MySQL
 Target Server Version : 50650
 File Encoding         : 65001

 Date: 22/05/2022 20:58:30
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for nasdaq
-- ----------------------------
DROP TABLE IF EXISTS `nasdaq`;
CREATE TABLE `nasdaq`  (
  `id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '索引',
  `create_user` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '修改人的名字',
  `action_name` varchar(4) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '动作种类\r\nA B C D E[E为自定义动作 = B+D]\r\nA 常闭\r\nB 常开\r\nC 半开\r\nD 自动\r\nE B+D\r\n ',
  `action_status` varchar(40) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '动作状态 0 执行 1 已执行',
  `update_time` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '更新时间 创建后 mcu执行完毕后更新状态',
  `creat_ip` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT '创建命令IP',
  `mcu_ip` varchar(20) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL COMMENT 'MCUIP\r\n',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 146 CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '车辆管理' ROW_FORMAT = Compact;

SET FOREIGN_KEY_CHECKS = 1;
