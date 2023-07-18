import cv2
import mediapipe as mp
import numpy as np
mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose

cap = cv2.VideoCapture(0)  #импорт видеопотока

counter = 0  #счётчик
stage = None #состояние сгибания (нейтральное)


def calculate_angle(a, b, c):
    a = np.array(a)  #начало
    b = np.array(b)  #вершина угла
    c = np.array(c)  #конец

    radians = np.arctan2(c[1] - b[1], c[0] - b[0]) - np.arctan2(a[1] - b[1], a[0] - b[0])  #вычисление угла в радианах
    angle = np.abs(radians * 180.0 / np.pi)  #перевод угла в градусы

    if angle > 180.0:
        angle = 360 - angle

    return angle

with mp_pose.Pose(min_detection_confidence=0.5, min_tracking_confidence=0.5) as pose:  #объявление переменной pose как скелетной модели с точностью обнаружения 0.5
    while cap.isOpened(): #пока камера открыта
        ret, frame = cap.read() #считывание кадров с камеры
        image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  #перевод изображений в RGB
        image.flags.writeable = False  #не сохраняем изображения
        results = pose.process(image)  #массив результатов
        image.flags.writeable = True  #сохраняем изображения
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)  #перевод изображений в BGR

        try:
            landmarks = results.pose_landmarks.landmark #опорные точки (суставы)

            shoulder = [landmarks[mp_pose.PoseLandmark.LEFT_SHOULDER.value].x, landmarks[mp_pose.PoseLandmark.LEFT_SHOULDER.value].y]  #координаты левого плеча
            elbow = [landmarks[mp_pose.PoseLandmark.LEFT_ELBOW.value].x, landmarks[mp_pose.PoseLandmark.LEFT_ELBOW.value].y]        #координаты левого локтя
            wrist = [landmarks[mp_pose.PoseLandmark.LEFT_WRIST.value].x, landmarks[mp_pose.PoseLandmark.LEFT_WRIST.value].y]        #координаты левого запястья

            angle = calculate_angle(shoulder, elbow, wrist)            #вычисление угла сгиба локтя
            #вывод строки с значением угла, рядом с локтем (умножаем расширение окна на координаты локтя), шрифт, размер, цвет
            cv2.putText(image, str(angle), tuple(np.multiply(elbow, [640, 480]).astype(int)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2, cv2.LINE_AA)

            if angle > 160:
                stage = "down"
            if angle < 30 and stage == 'down':
                stage = "up"
                counter += 1

        except:
            pass
        #прямоугольник состояния: начальные координаты, размер, цвет, заполнение цветом (-1))
        cv2.rectangle(image, (0, 0), (225, 73), (245, 117, 16), -1)

        # вывод количества повторений (начальные координаты, шрифт, размер шрифта, цвет, толщина линий, тип линии)
        cv2.putText(image, 'REPS', (15, 12), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv2.LINE_AA)
        cv2.putText(image, str(counter), (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 1, cv2.LINE_AA)

        # вывод состояния сгибания (вверх, вниз)
        cv2.putText(image, 'STAGE', (100, 12), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv2.LINE_AA)
        cv2.putText(image, stage, (80, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 1, cv2.LINE_AA)

        mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS,
                                  mp_drawing.DrawingSpec(color=(245, 117, 66), thickness=2, circle_radius=2),
                                  mp_drawing.DrawingSpec(color=(245, 66, 230), thickness=2, circle_radius=2)
                                  )  #отрисовка линий, соединяющих опорные точки позы человека

        cv2.imshow('Mediapipe Feed', image)  #всплывающее окно с кадром

        if cv2.waitKey(10) & 0xFF == ord('q'):  #выход из программы
            break

    cap.release()  #закрытие камеры
    cv2.destroyAllWindows()  #закрытие всех окон