ALTER TABLE `banks` ADD INDEX `BANKDELETED`(`deleted_at`);
ALTER TABLE `categories` ADD INDEX `CATEGORYDELETED`(`deleted_at`);
ALTER TABLE `customers` ADD INDEX `CUSTOMERDELETED`(`deleted_at`);
ALTER TABLE `items` ADD INDEX `ITEMDELETED`(`deleted_at`);
ALTER TABLE `machines` ADD INDEX `MACHINEDELETED`(`deleted_at`);
ALTER TABLE `supliers` ADD INDEX `SUPLIERDELETED`(`deleted_at`);
ALTER TABLE `users` ADD INDEX `USERDELETED`(`deleted_at`);