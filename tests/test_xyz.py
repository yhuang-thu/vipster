from io import StringIO
from vipster.ioplugins import xyz
from vipster import Molecule
from test_preamble import atom_equal


def test_xyz_parse_single():
    name = 'xyz_single'
    target = ["3\n",
              "\n",
              "O   0.0 0.0 0.0\n",
              "H   0.9572 0.0 0.0\n",
              "H   -0.23999 0.926627 0.0"]
    Mol, _ = xyz.parser(name, target)
    assert Mol.name == name
    assert Mol.nat == 3
    assert Mol.ntyp == 2
    assert Mol.getTypes() == ['H', 'O']
    assert Mol.getFmt() == 'angstrom'
    assert atom_equal(Mol.getAtom(0, charge=True, fix=True, hidden=True),
                      ['O', (0, 0, 0), '0.',
                       [False, False, False], False])
    assert atom_equal(Mol.getAtom(2, fmt='bohr'),
                      ['H', (-0.4535, 1.751, 0)])
    assert len(Mol.getBonds(1.1)[0]) == 2


def test_xyz_parse_multi():
    name = 'xyz_multi'
    target = ["3\n",
              "\n",
              "O   0.0 0.0 0.0\n",
              "H   0.9572 0.0 0.0\n",
              "H   -0.23999 0.926627 0.0\n",
              "\n",
              "5\n",
              "\n",
              "C   0.0 0.0 0.0\n",
              "H   1.1 0.0 0.0\n",
              "H   -0.366667 1.03709 0.0\n",
              "H   -0.366667 -0.518545 0.898146\n",
              "H   -0.366667 -0.518545 -0.898146\n"]
    Mol, _ = xyz.parser(name, target)
    assert Mol.name == name
    assert len(Mol) == 2
    assert Mol.nat == 5
    assert Mol.ntyp == 2
    assert Mol.getFmt() == 'angstrom'
    assert len(Mol.getBonds(1.1)[0]) == 4
    assert Mol.getTypes() == ['H', 'C']
    Mol.changeStep(0)
    assert Mol.nat == 3
    assert Mol.ntyp == 2
    assert len(Mol.getBonds(1.1)[0]) == 2
    assert Mol.getTypes() == ['H', 'O']


def test_xyz_write():
    target = "3\n"\
             "\n"\
             "O     0.0000  0.0000  0.0000\n"\
             "H     0.9572  0.0000  0.0000\n"\
             "H    -0.2400  0.9266  0.0000\n"
    f = StringIO()
    Mol = Molecule()
    Mol.newAtom('O', (0, 0, 0), fmt='angstrom')
    Mol.newAtom('H', (0.9572, 0, 0), fmt='angstrom')
    Mol.newAtom('H', (-0.23999, 0.926627, 0), fmt='angstrom')
    xyz.writer(Mol, f, None)
    assert f.getvalue() == target
