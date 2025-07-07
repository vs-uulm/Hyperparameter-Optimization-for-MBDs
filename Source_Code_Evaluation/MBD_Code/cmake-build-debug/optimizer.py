import optuna
import subprocess
import shlex

def objective(trial):
    var_range_plausibility = trial.suggest_float('varRangePlausibility', 2, 20)
    var_speed_plausibility = trial.suggest_float('varSpeedPlausibility', 0.02, 0.1)
    var_position_consistency = trial.suggest_float('varPositionConsistency', 0.1, 0.4)
    var_speed_consistency = trial.suggest_float('varSpeedConsistency', 0.02, 0.1)
    var_position_heading_consistency = trial.suggest_float('varPositionHeadingConsistency', 5.0, 20.0)
    var_intersection_check = trial.suggest_float('varIntersectionCheck', 0.0001, 0.01)
    var_sudden_appearance_check = trial.suggest_float('varSuddenAppearanceCheck', 2.0, 10.0)
    var_rtl = trial.suggest_float('varRTL', 0.6, 0.9)

    command = (
        f"./legacy_detectors "
        f"-r {var_range_plausibility} "
        f"-s {var_speed_plausibility} "
        f"-p {var_position_consistency} "
        f"-c {var_speed_consistency} "
        f"-h {var_position_heading_consistency} "
        f"-i {var_intersection_check} "
        f"-a {var_sudden_appearance_check} "
        f"-a {var_rtl} "
    )
    
    process = subprocess.run(command.split(" "), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    

    output = process.stdout.decode('utf-8').strip().splitlines()
    score = float(output[-1])

    return score

study = optuna.create_study(direction='maximize')
study.optimize(objective, n_trials=300)

print("Best parameters:", study.best_params)
print("Best score:", study.best_value)
