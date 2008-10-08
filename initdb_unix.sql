BEGIN;

DELETE FROM participants;
INSERT INTO participants (name, first_name, number) VALUES ('Huber', 'Stefan', 1);
INSERT INTO participants (name, first_name, number) VALUES ('Brion', 'Jesica', 3);
INSERT INTO participants (name, first_name, number) VALUES ('Weber', 'Jens', 4);
INSERT INTO participants (name, first_name, number) VALUES ('Broy', 'Nora', 5);
INSERT INTO participants (name, first_name, number) VALUES ('Heck', 'Tobias', 6);
INSERT INTO participants (name, first_name, number) VALUES ('Liegsalz', 'Florian', 7);
INSERT INTO participants (name, first_name, number) VALUES ('Zec', 'Marin', 8);
INSERT INTO participants (name, first_name, number) VALUES ('Bunke', 'Friedl', 9);
INSERT INTO participants (name, first_name, number) VALUES ('Kluegel', 'Niklas', 10);
INSERT INTO participants (name, first_name, number) VALUES ('Dirding', 'Philipp', 12);
INSERT INTO participants (name, first_name, number) VALUES ('Hauptmann', 'Claudius', 15);
INSERT INTO participants (name, first_name, number) VALUES ('Greben', 'Konstantin', 16);
INSERT INTO participants (name, first_name, number) VALUES ('Philipp', 'Andy', 17);
INSERT INTO participants (name, first_name, number) VALUES ('Bauer', 'Bernhard', 18);
INSERT INTO participants (name, first_name, number) VALUES ('Schiel', 'Andrea', 19);
INSERT INTO participants (name, first_name, number) VALUES ('Eigner', 'Robert', 20);
INSERT INTO participants (name, first_name, number) VALUES ('Kramm', 'Matthias', 21);
INSERT INTO participants (name, first_name, number) VALUES ('Brocco', 'Michele', 22);
INSERT INTO participants (name, first_name, number) VALUES ('Roth', 'Lilo', 23);
INSERT INTO participants (name, first_name, number) VALUES ('Schlichter', 'Johann', 24);
INSERT INTO participants (name, first_name, number) VALUES ('Rothlehner', 'Stefan', 25);
INSERT INTO participants (name, first_name, number) VALUES ('Baucks', 'Conrad', 26);
INSERT INTO participants (name, first_name, number) VALUES ('Peppinghaus', 'Sascha', 27);
INSERT INTO participants (name, first_name, number) VALUES ('Kahl', 'Vincent', 28);
INSERT INTO participants (name, first_name, number) VALUES ('Endriss', 'Philipp', 29);

DELETE FROM samples;
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 1), '/usr/local/Experiment/Samples/teiln01_000202.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 3), '/usr/local/Experiment/Samples/teiln03_000207.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 4), '/usr/local/Experiment/Samples/teiln04_000200.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 5), '/usr/local/Experiment/Samples/teiln05_000026.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 6), '/usr/local/Experiment/Samples/teiln06_000211.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 7), '/usr/local/Experiment/Samples/teiln07_000222.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 8), '/usr/local/Experiment/Samples/teiln08_000121.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 9), '/usr/local/Experiment/Samples/teiln09_000142.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 10), '/usr/local/Experiment/Samples/teiln10_000129.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 12), '/usr/local/Experiment/Samples/teiln12_000133.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 15), '/usr/local/Experiment/Samples/teiln15_000152.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 16), '/usr/local/Experiment/Samples/teiln16_000202.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 17), '/usr/local/Experiment/Samples/teiln17_000156.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 18), '/usr/local/Experiment/Samples/teiln18_000224.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 19), '/usr/local/Experiment/Samples/teiln19_000218.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 20), '/usr/local/Experiment/Samples/teiln20_000155.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 21), '/usr/local/Experiment/Samples/teiln21_000201.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 22), '/usr/local/Experiment/Samples/teiln22_000200.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 23), '/usr/local/Experiment/Samples/teiln23_000131.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 24), '/usr/local/Experiment/Samples/teiln24_000201.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 25), '/usr/local/Experiment/Samples/teiln25_000223.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 26), '/usr/local/Experiment/Samples/teiln26_000206.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 27), '/usr/local/Experiment/Samples/teiln27_000206.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 28), '/usr/local/Experiment/Samples/teiln28_000113.wav');
INSERT INTO SAMPLES (participant_id, filename) VALUES ((SELECT ID FROM participants WHERE number = 29), '/usr/local/Experiment/Samples/teiln29_000138.wav');

DELETE FROM movies;
INSERT INTO movies VALUES ('/usr/local/Experiment/Einzelbilder/Camcorder_Matthias/Camcorder_Matthias_', '.jpg', 0, 3599, 500, 5, 'Camcorder Matthias');
INSERT INTO movies VALUES ('/usr/local/Experiment/Einzelbilder/Camcorder_Alex/Camcorder_Alex_', '.jpg', 0, 3599, 500, 5, 'Camcorder Alex');
INSERT INTO movies VALUES ('/usr/local/Experiment/Einzelbilder/Grosse_Kamera/Grosse_Kamera_', '.jpg', 0, 3599, 500, 5, 'Grosse Kamera');
INSERT INTO movies VALUES ('/usr/local/Experiment/Einzelbilder/Mobile_Kamera/Mobile_Kamera_', '.jpg', 0, 3599, 500, 5, 'Mobile Kamera');
INSERT INTO movies VALUES ('/usr/local/Experiment/Einzelbilder/Webcam_Georg/Webcam_Georg_', '.jpg', 0, 3599, 500, 5, 'Webcam Georg');
INSERT INTO movies VALUES ('/usr/local/Experiment/Einzelbilder/Bruegge/Bruegge_', '.jpg', 0, 3599, 500, 5, 'Bruegge');

DELETE FROM sender;
DELETE FROM attributes;

COMMIT;
