import numpy as np

def simplex_method(c, A, b):
    """
    Реализация симплекс-метода для задачи линейного программирования.
    :param c: коэффициенты целевой функции (список)
    :param A: матрица коэффициентов ограничений (список списков)
    :param b: правая часть ограничений (список)
    :return: Оптимальное решение или сообщение об ошибке
    """
    num_vars = len(c)
    num_constraints = len(b)
    
    # Создаем симплекс-таблицу
    simplexTable = np.zeros((num_constraints + 1, num_vars + num_constraints + 1))
    
    # Строим табличку, сразу приводим к канонической форме
    simplexTable[:-1, :num_vars] = A
    simplexTable[:-1, num_vars:num_vars + num_constraints] = np.eye(num_constraints) # Добавляем доп переменные
    simplexTable[:-1, -1] = b
    
    # Заполняем строку целевой функции
    simplexTable[-1, :num_vars] = -np.array(c)
   
    # Задача сформулирована так, что коэффициенты в ограничениях и x-ах у нас неотрицательные,
    # поэтому большую часть действий по приведению задачи к канонической форме пропускаем 

    while np.any(simplexTable[-1, :-1] < 0):
        pivot_col = np.argmin(simplexTable[-1, :-1]) # ищем наименьший коэффициент отрицательной функции
        ratios = simplexTable[:-1, -1] / simplexTable[:-1, pivot_col]
        ratios[simplexTable[:-1, pivot_col] <= 0] = np.inf
        pivot_row = np.argmin(ratios)
        
        if np.all(ratios == np.inf):
            return {'Ошибка': 'Задача неограничена'}
        
        pivot_value = simplexTable[pivot_row, pivot_col]
        simplexTable[pivot_row, :] /= pivot_value
        
        for i in range(num_constraints + 1):
            if i != pivot_row:
                simplexTable[i, :] -= simplexTable[i, pivot_col] * simplexTable[pivot_row, :]
    
    solution = np.zeros(num_vars)
    for i in range(num_constraints):
        basic_var = np.where(simplexTable[i, :num_vars] == 1)[0]
        if len(basic_var) == 1:
            solution[basic_var[0]] = simplexTable[i, -1]
    
    return {
        'Маги': solution[0],
        'Рыцари': solution[1],
        'Драконы': solution[2],
        'Максимальная сила': simplexTable[-1, -1]
    }

def maximize_army_strength(m, k, d, r, p):
    """
    Решает задачу максимизации общей силы армии.
    :param m: список [m1, m2, m3] - стоимость мага в каждом ресурсе
    :param k: список [k1, k2, k3] - стоимость рыцаря в каждом ресурсе
    :param d: список [d1, d2, d3] - стоимость дракона в каждом ресурсе
    :param r: список [r1, r2, r3] - доступные запасы ресурсов
    :param p: список [p1, p2, p3] - сила магов, рыцарей и драконов
    :return: Оптимальное количество магов, рыцарей, драконов и максимальная сила
    """
    c = [p[0], p[1], p[2]]
    A = np.array([m, k, d]).T
    b = r
    return simplex_method(c, A, b)

# Пример вызова
if __name__ == "__main__":
    m = [10, 0, 0]
    k = [0, 10, 0]
    d = [100, 100, 100]
    r = [220, 220, 220]
    p = [11, 10, 1000]
    
    solution = maximize_army_strength(m, k, d, r, p)
    print(solution)
