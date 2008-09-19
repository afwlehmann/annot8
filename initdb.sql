BEGIN;

DELETE FROM participants;
INSERT INTO participants (name, first_name, number) VALUES ('Lehmann', 'Alexander', 1);
INSERT INTO participants (name, first_name, number) VALUES ('Groh', 'Georg', 3);
INSERT INTO participants (name, first_name, number) VALUES ('Lustig', 'Peter', 4);
INSERT INTO participants (name, first_name, number) VALUES ('Sifredi', 'Rocco', 5);
INSERT INTO participants (name, first_name, number) VALUES ('North', 'Peter', 6);
INSERT INTO participants (name, first_name, number) VALUES ('Midwest', 'Melissa', 7);
INSERT INTO participants (name, first_name, number) VALUES ('Spears', 'Britney', 8);
INSERT INTO participants (name, first_name, number) VALUES ('Merkel', 'Angela', 9);

DELETE FROM movies;
INSERT INTO movies VALUES ('/Users/alex/Desktop/HiWi/Experiment/Einzelbilder/Camcorder_Alex/Camcorder_Alex_', '.jpg', 1, 7200, 250, 5, 'Camcorder Alex');
INSERT INTO movies VALUES ('/Users/alex/Desktop/HiWi/Experiment/Einzelbilder/Camcorder_Matthias/Camcorder_Matthias_', '.jpg', 1, 7200, 250, 5, 'Camcorder Matthias');
INSERT INTO movies VALUES ('/Users/alex/Desktop/HiWi/Experiment/Einzelbilder/Grosse_Kamera/Grosse_Kamera_', '.jpg', 1, 7200, 250, 5, 'Grosse Kamera');
INSERT INTO movies VALUES ('/Users/alex/Desktop/HiWi/Experiment/Einzelbilder/Mobile_Kamera/Mobile_Kamera_', '.jpg', 1, 7200, 250, 5, 'Mobile Kamera');
INSERT INTO movies VALUES ('/Users/alex/Desktop/HiWi/Experiment/Einzelbilder/Webcam_Georg/Webcam_Georg_', '.jpg', 1, 7200, 250, 5, 'Webcam Georg');
-- INSERT INTO movies VALUES ('Z:\Einzelbilder\Camcorder_Alex\Camcorder_Alex_', '.jpg', 1, 7200, 250, 5, 'Camcorder Alex');
-- INSERT INTO movies VALUES ('Z:\Einzelbilder\Camcorder_Matthias\Camcorder_Matthias_', '.jpg', 1, 7200, 250, 5, 'Camcorder Matthias');
-- INSERT INTO movies VALUES ('Z:\Einzelbilder\Grosse_Kamera\Grosse_Kamera_', '.jpg', 1, 7200, 250, 5, 'Grosse Kamera');
-- INSERT INTO movies VALUES ('Z:\Einzelbilder\Mobile_Kamera\Mobile_Kamera_', '.jpg', 1, 7200, 250, 5, 'Mobile Kamera');
-- INSERT INTO movies VALUES ('Z:\Einzelbilder\Webcam_Georg\Webcam_Georg_', '.jpg', 1, 7200, 250, 5, 'Webcam Georg');

DELETE FROM samples;
INSERT INTO samples (participant_id, filename) VALUES ((SELECT id FROM participants WHERE name LIKE 'Lehmann'), '/Users/alex/Desktop/HiWi/Experiment/Samples/teiln01_000133_003233.wav');
INSERT INTO samples (participant_id, filename) VALUES ((SELECT id FROM participants WHERE name LIKE 'Groh'), '/Users/alex/Desktop/HiWi/Experiment/Samples/teiln03_000138_003238.wav');
INSERT INTO samples (participant_id, filename) VALUES ((SELECT id FROM participants WHERE name LIKE 'Lustig'), '/Users/alex/Desktop/HiWi/Experiment/Samples/teiln04_000130_003230.wav');
-- INSERT INTO samples (participant_id, filename) VALUES ((SELECT id FROM participants WHERE name LIKE 'Lehmann'), 'Z:\Samples\teiln01_000133_003233.wav');
-- INSERT INTO samples (participant_id, filename) VALUES ((SELECT id FROM participants WHERE name LIKE 'Groh'),    'Z:\Samples\teiln03_000138_003238.wav');
-- INSERT INTO samples (participant_id, filename) VALUES ((SELECT id FROM participants WHERE name LIKE 'Lustig'),  'Z:\Samples\teiln04_000130_003230.wav');

DELETE FROM sender;

COMMIT;
