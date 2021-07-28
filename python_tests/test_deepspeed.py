import pytest
import os
os.chdir('{}/cluster/custom_loader/third_party/DeepSpeed'.format(os.environ['HOME']))
pytest.main()
