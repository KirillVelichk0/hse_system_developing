import numpy as np

def simplex_method(c, A, b):
    """
    Реализация симплекс-метода для задачи линейного программирования.
    :param c: коэффициенты целевой функции (список)
    :param A: матрица коэффициентов ограничений (список списков)
    :param b: правая часть ограничений (список)
    :return: Оптимальное решение или сообщение об ошибке
    """
    num_vars = len(c)  # Количество переменных (x1, x2, x3)
    num_constraints = len(b)  # Количество ограничений
    
    # Создаем симплекс-таблицу (матрица коэффициентов + доп. переменные)
    tableau = np.zeros((num_constraints + 1, num_vars + num_constraints + 1))
    
    # Заполняем матрицу ограничений в таблице
    tableau[:-1, :num_vars] = A
    tableau[:-1, num_vars:num_vars + num_constraints] = np.eye(num_constraints)  # Добавляем искусственные переменные
    tableau[:-1, -1] = b  # Заполняем правую часть (свободные члены)
    
    # Заполняем строку целевой функции (коэффициенты Z)
    tableau[-1, :num_vars] = -np.array(c)
    
    # Основной цикл симплекс-метода
    while np.any(tableau[-1, :-1] < 0):  # Пока есть отрицательные коэффициенты в последней строке
        
        # Выбираем ведущий столбец (самый отрицательный коэффициент в Z)
        pivot_col = np.argmin(tableau[-1, :-1])
        
        # Вычисляем допустимые отношения, избегая деления на ноль
        valid_rows = tableau[:-1, pivot_col] > 0
        ratios = np.full(len(b), np.inf, dtype=float)
        ratios[valid_rows] = tableau[valid_rows, -1] / tableau[valid_rows, pivot_col]
        
        pivot_row = np.argmin(ratios)
        
        # Проверяем, не является ли задача неограниченной
        if np.all(ratios == np.inf):
            return {'Ошибка': 'Задача неограничена'}
        
        # Приводим ведущий элемент к 1
        pivot_value = tableau[pivot_row, pivot_col]
        tableau[pivot_row, :] /= pivot_value
        
        # Обновляем остальные строки
        for i in range(num_constraints + 1):
            if i != pivot_row:
                tableau[i, :] -= tableau[i, pivot_col] * tableau[pivot_row, :]
    
    # Извлекаем решение
    solution = np.zeros(num_vars)
    for i in range(num_constraints):
        basic_var = np.where(tableau[i, :num_vars] == 1)[0]  # Ищем базисные переменные
        if len(basic_var) == 1:
            solution[basic_var[0]] = tableau[i, -1]
    
    return solution, tableau[-1, -1]

def branch_and_bound(c, A, b):
    """
    Метод ветвей и границ для целочисленного линейного программирования.
    """
    best_solution = None
    best_value = -np.inf
    queue = [(A, b, c)]  # Очередь задач для решения
    
    while queue:
        A_curr, b_curr, c_curr = queue.pop()
        solution, value = simplex_method(c_curr, A_curr, b_curr)
        
        if solution is None:
            continue  # Пропускаем неразрешимые задачи
        
        if np.all(np.floor(solution) == solution):  # Проверяем целочисленность
            if value > best_value:
                best_solution = solution
                best_value = value
        else:
            fractional_index = np.argmax(solution - np.floor(solution))
            
            floor_val = np.floor(solution[fractional_index])
            ceil_val = np.ceil(solution[fractional_index])
            
            A_lower = np.vstack([A_curr, np.eye(1, len(c), fractional_index)])
            b_lower = np.append(b_curr, floor_val)
            queue.append((A_lower, b_lower, c_curr))
            
            A_upper = np.vstack([A_curr, -np.eye(1, len(c), fractional_index)])
            b_upper = np.append(b_curr, -ceil_val)
            queue.append((A_upper, b_upper, c_curr))
    
    return best_solution, best_value

def maximize_army_strength(m, k, d, r, p):
    """
    Решает задачу максимизации общей силы армии.
    """
    c = [p[0], p[1], p[2]]  # Коэффициенты целевой функции
    A = [m, k, d]  # Матрица коэффициентов ограничений
    b = r  # Свободные члены
    return branch_and_bound(c, A, b)

# Пример вызова
if __name__ == "__main__":
    m = [1, 2, 3]  # Потребление магами ресурсов
    k = [2, 1, 3]  # Потребление рыцарями ресурсов
    d = [3, 3, 1]  # Потребление драконами ресурсов
    r = [100, 50, 30]  # Доступные запасы ресурсов
    p = [10, 20, 50]  # Сила существ
    
    solution, max_strength = maximize_army_strength(m, k, d, r, p)
    print("Оптимальное распределение:", solution)
    print("Максимальная сила:", max_strength)

